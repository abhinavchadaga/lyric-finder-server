# syntax=docker/dockerfile:1
FROM nvidia/cuda:11.7.1-devel-ubuntu22.04 AS builder

# install build dependencies
RUN apt-get update && \
    apt-get install -y --no-install-recommends \
    cmake \
    git \
    gdb \
    libboost-all-dev

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
    cmake -DCMAKE_BUILD_TYPE=Release .. && \
    cmake --build .

ENTRYPOINT ["/app/build/server"]

#################################################################################
#FROM alpine:3.19.0 AS runtime
#
### Create a non-privileged user that the app will run under.
### See https://docs.docker.com/go/dockerfile-user-best-practices/
#ARG UID=10001
#RUN adduser \
#    --disabled-password \
#    --gecos "" \
#    --home "/nonexistent" \
#    --shell "/sbin/nologin" \
#    --no-create-home \
#    --uid "${UID}" \
#    appuser
#
## Set the working directory
#WORKDIR /app
#
### Copy the executable from the "build" stage.
#COPY --from=builder /app/build/server /app/
#
## Set permissions
#RUN chown appuser:appuser /app/server && \
#    chmod +x /app/server  # Ensure the server is executable
#
#USER appuser
#
## Set the entrypoint to your application executable
#ENTRYPOINT ["/app/server"]
