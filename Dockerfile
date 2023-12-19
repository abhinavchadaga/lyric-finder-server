# syntax=docker/dockerfile:1
FROM nvidia/cuda:11.7.1-devel-ubuntu22.04 AS builder

# install build dependencies
RUN apt-get update && \
    apt-get install -y --no-install-recommends \
    cmake \
    git \
    gdb \
    libboost-all-dev \
    ninja-build

# install oat++
RUN git clone https://github.com/oatpp/oatpp.git \
    && cd oatpp \
    && mkdir build \
    && cd build \
    && cmake -DCMAKE_BUILD_TYPE=Release -DOATPP_BUILD_TESTS=OFF .. \
    && make install

# copy source code
WORKDIR /app
COPY . .

# Build the application
RUN mkdir build && \
    cd build && \
    cmake -DCMAKE_BUILD_TYPE=Release -G Ninja .. && \
    cmake --build .

#################################################################################
FROM nvidia/cuda:11.7.1-runtime-ubuntu22.04 AS runtime

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
COPY --from=builder /app/build/server /app/
COPY --from=builder /app/db/ /app/db/

# Set permissions
RUN chown appuser:appuser /app/server && \
    chmod +x /app/server

USER appuser

EXPOSE 8000

# Set the entrypoint to your application executable
ENTRYPOINT ["/app/server"]
