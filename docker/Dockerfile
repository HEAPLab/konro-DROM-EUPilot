# Dev environment for Konro + DLB (DROM + TALP). Works on x86_64 and arm64.
# Build: docker build -t konro-talp -f docker/Dockerfile .
# Run:   docker run --rm -it --name konro-talp --shm-size=256m --privileged -v "$PWD":/workspace konro-talp
#        (--privileged: Konro moves each managed app into its own cgroup under /sys/fs/cgroup)
FROM ubuntu:24.04
ARG DLB_VERSION=v3.8.0
COPY docker/install-deps.sh /tmp/install-deps.sh
RUN DLB_VERSION=${DLB_VERSION} bash /tmp/install-deps.sh && rm /tmp/install-deps.sh
ENV CC=clang CXX=clang++
WORKDIR /workspace
CMD ["/bin/bash"]