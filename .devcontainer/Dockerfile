FROM ubuntu:22.04

# Development APT packages
RUN apt -qq update && \
    apt -qq install -y --no-install-recommends \
        sudo \
        git \
        wget \
        curl \
        zip \
        unzip \
        tar \
        ca-certificates \
        build-essential \
        gcc \
        g++ \
        gdb \
        make \
        ninja-build \
        autoconf \
        automake \
        locales-all \
        dos2unix \
        rsync \
        tar \
        ssh \
        python3 \
        && \
    apt -qq clean && \
    apt -qq autoclean && \
    apt -qq remove && \
    apt -qq autoremove

# Install CMAKE
RUN apt update && \
    cd /tmp && \
    wget --no-check-certificate https://github.com/Kitware/CMake/releases/download/v3.25.0/cmake-3.25.0-linux-x86_64.tar.gz && \
    tar -xzvf cmake-3.25.0-linux-x86_64.tar.gz && \
    mv cmake-3.25.0-linux-x86_64 /opt/cmake && \
    rm cmake-3.25.0-linux-x86_64.tar.gz
ENV PATH="/opt/cmake/bin:${PATH}"++

# Install VCPKG
RUN apt update && \
    cd /tmp && \
    wget --no-check-certificate https://github.com/microsoft/vcpkg/archive/refs/tags/2023.02.24.tar.gz && \
    tar -xzvf 2023.02.24.tar.gz && \
    mv vcpkg-2023.02.24 /opt/vcpkg && \
    rm 2023.02.24.tar.gz && \
    cd /opt/vcpkg && \
    ./bootstrap-vcpkg.sh
ENV PATH="/opt/vcpkg:${PATH}"

# Download Clang 15.0.7
RUN apt update && \
    cd /tmp && \
    curl -LO https://github.com/llvm/llvm-project/archive/refs/tags/llvmorg-15.0.7.tar.gz && \
    tar -xvf llvmorg-15.0.7.tar.gz && \
    rm llvmorg-15.0.7.tar.gz

# Build Clang 15.0.7
RUN apt update && \
    cd /tmp/llvm-project-llvmorg-15.0.7 && \
    mkdir build && cd build && \
    cmake -DLLVM_ENABLE_PROJECTS=clang -DCMAKE_BUILD_TYPE=Release -G "Unix Makefiles" ../llvm && \
    make clang -j $(nproc)

# Install Clang 15.0.7
RUN cd /tmp/llvm-project-llvmorg-15.0.7 && \
    mv build /opt/clang-15.0.7
ENV PATH="/opt/clang-15.0.7/bin:${PATH}"



