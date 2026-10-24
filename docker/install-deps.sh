#!/usr/bin/env bash
# Installs all build dependencies for Konro + DLB (DROM + TALP) on Ubuntu 24.04.
set -euo pipefail

DLB_VERSION="${DLB_VERSION:-v3.8.0}"
export DEBIAN_FRONTEND=noninteractive

# clang + LLVM OpenMP (libomp): DLB follows OpenMP apps through OMPT,
# which GNU libgomp does not implement.
apt-get update
apt-get install -y --no-install-recommends \
    build-essential cmake pkg-config git ca-certificates \
    autoconf automake libtool python3 python3-matplotlib \
    clang libomp-dev \
    liblog4cpp5-dev libhwloc-dev libsensors-dev \
    procps less vim gdb
rm -rf /var/lib/apt/lists/*

# DLB from source into /usr/local (Konro links /usr/local/lib/libdlb.so)
src="$(mktemp -d)"
git clone --depth 1 --branch "${DLB_VERSION}" https://github.com/bsc-pm/dlb.git "$src"
(cd "$src" && ./bootstrap && ./configure --prefix=/usr/local CC=clang CXX=clang++ \
    && make -j"$(nproc)" && make install)
ldconfig
rm -rf "$src"