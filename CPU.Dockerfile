FROM ubuntu:22.04 as builder
LABEL authors="achadaga"

RUN apt-get update && \
    apt-get install -y --no-install-recommends \
    build-essential \
    cmake \
    git \
    gdb \
    libboost-all-dev \
    ca-certificates \
    ninja-build

# copy source code
WORKDIR /app
COPY . .

# Build the application
RUN mkdir Release && \
    cd Release && \
    cmake -DCMAKE_BUILD_TYPE=Release -G Ninja .. && \
    cmake --build .

#################################################################################
FROM ubuntu:22.04 AS runtime

### Create a non-privileged user that the app will run under.
### See https://docs.docker.com/go/dockerfile-user-best-practices/
ARG UID=10001
RUN adduser \
    --disabled-password \
    --gecos "" \
    --home "/nonexistent" \
    --shell "/sbin/nologin" \
    --no-create-home \
    --uid "${UID}" \
    appuser

# Set the working directory
WORKDIR /app

## Copy executable and db file from builder stage
COPY --from=builder /app/Release/lyric-finder-server /app/Release/lyric-finder-server
COPY --from=builder /app/db/ /app/db/
COPY --from=builder /app/dist/ /app/dist/

# Set permissions
RUN chown appuser:appuser /app/Release/lyric-finder-server && \
    chmod +x /app/Release/lyric-finder-server

USER appuser

EXPOSE 8000

WORKDIR /app/Release/

# Set the entrypoint to your application executable
ENTRYPOINT ["/app/Release/lyric-finder-server"]
