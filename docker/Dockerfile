FROM ubuntu:22.04

LABEL maintainer="ToolController Embedded Team"
LABEL description="Reproducible Build & Test Environment for ToolController (STM32H7 + Unity + gcovr)"

ENV DEBIAN_FRONTEND=noninteractive

# 1. Cài đặt các công cụ cơ bản và biên dịch host
RUN apt-get update && apt-get install -y --no-install-recommends \
    build-essential \
    make \
    ninja-build \
    gcc-12 \
    g++-12 \
    gdb-multiarch \
    git \
    wget \
    curl \
    xz-utils \
    python3 \
    python3-pip \
    ca-certificates \
    && rm -rf /var/lib/apt/lists/*

# 2. Đặt gcc-12 làm trình biên dịch mặc định cho host
RUN update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-12 100 \
    && update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-12 100

# 3. Cài đặt ARM GNU Toolchain chính thức từ Arm (phiên bản 13.3.rel1)
ARG ARM_VERSION=13.3.rel1
RUN wget -q https://developer.arm.com/-/media/Files/downloads/gnu/${ARM_VERSION}/binrel/arm-gnu-toolchain-${ARM_VERSION}-x86_64-arm-none-eabi.tar.xz \
    && tar -xf arm-gnu-toolchain-${ARM_VERSION}-x86_64-arm-none-eabi.tar.xz -C /opt \
    && rm arm-gnu-toolchain-${ARM_VERSION}-x86_64-arm-none-eabi.tar.xz

ENV PATH="/opt/arm-gnu-toolchain-${ARM_VERSION}-x86_64-arm-none-eabi/bin:${PATH}"

# 4. Cài đặt CMake mới nhất và gcovr để xuất báo cáo HTML coverage
RUN pip3 install --no-cache-dir cmake gcovr

# 5. Thư mục làm việc trong container
WORKDIR /workspace/ToolController

