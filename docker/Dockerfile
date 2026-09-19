# syntax=docker/dockerfile:1.4

# ==============================================================================
# STAGE 1: Tải và giải nén ARM GNU Toolchain song song (BuildKit Parallel Stage)
# ==============================================================================
FROM ubuntu:22.04 AS toolchain-downloader

ARG ARM_VERSION=13.3.rel1
ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y --no-install-recommends \
    wget \
    xz-utils \
    ca-certificates \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /tmp
RUN wget -q --show-progress --progress=dot:giga \
    https://developer.arm.com/-/media/Files/downloads/gnu/${ARM_VERSION}/binrel/arm-gnu-toolchain-${ARM_VERSION}-x86_64-arm-none-eabi.tar.xz \
    && mkdir -p /opt/arm-toolchain \
    && tar -xf arm-gnu-toolchain-${ARM_VERSION}-x86_64-arm-none-eabi.tar.xz --strip-components=1 -C /opt/arm-toolchain \
    && rm arm-gnu-toolchain-${ARM_VERSION}-x86_64-arm-none-eabi.tar.xz

# ==============================================================================
# STAGE 2: Base System & Compiler Dependencies
# ==============================================================================
FROM ubuntu:22.04 AS final

LABEL maintainer="ToolController Embedded Team"
LABEL description="Optimized Reproducible Build & Test Environment for ToolController"

ENV DEBIAN_FRONTEND=noninteractive
ENV CCACHE_DIR=/tmp/.ccache

# 1. Cài đặt các công cụ biên dịch host, ninja, ccache với APT Cache Mount
RUN --mount=type=cache,target=/var/cache/apt,sharing=locked \
    --mount=type=cache,target=/var/lib/apt,sharing=locked \
    apt-get update && apt-get install -y --no-install-recommends \
    build-essential \
    make \
    ninja-build \
    gcc-12 \
    g++-12 \
    ccache \
    git \
    python3 \
    python3-pip \
    ca-certificates

# 2. Đặt gcc-12 làm trình biên dịch mặc định cho host
RUN update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-12 100 \
    && update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-12 100

# 3. Cài đặt CMake và gcovr với PIP Cache Mount
RUN --mount=type=cache,target=/root/.cache/pip \
    pip3 install cmake gcovr

# 4. Sao chép ARM GNU Toolchain từ STAGE 1 (chạy song song, không tốn thời gian chờ)
COPY --from=toolchain-downloader /opt/arm-toolchain /opt/arm-toolchain

ENV PATH="/opt/arm-toolchain/bin:${PATH}"

# 5. Thư mục làm việc trong container
WORKDIR /workspace/ToolController
