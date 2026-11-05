# Development environment for Konro + DLB (DROM + TALP), x86_64 and arm64.
# Build: docker build -t konro-talp -f docker/Dockerfile .
# Run: docker run --rm -it --name konro-talp --shm-size=256m --privileged \
#          -v "$PWD":/workspace konro-talp
#   --shm-size: DLB keeps its shared memory in /dev/shm (64 MB by default)
#   --privileged: Konro moves each managed application into its own cgroup
FROM ubuntu:24.04

ARG DLB_VERSION=v3.8.0
COPY docker/install-deps.sh /tmp/install-deps.sh
RUN DLB_VERSION=${DLB_VERSION} bash /tmp/install-deps.sh \
    && rm -rf /tmp/install-deps.sh /var/lib/apt/lists/*

ENV CC=clang CXX=clang++
WORKDIR /workspace
CMD ["/bin/bash"]
