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
        && \
    apt -qq clean && \
    apt -qq autoclean && \
    apt -qq remove && \
    apt -qq autoremove

# Install CMAKE
RUN apt-get update && \
    apt-get install -y wget && \
    cd /tmp && \
    wget --no-check-certificate https://github.com/Kitware/CMake/releases/download/v3.25.0/cmake-3.25.0-linux-x86_64.tar.gz && \
    tar -xzvf cmake-3.25.0-linux-x86_64.tar.gz && \
    mv cmake-3.25.0-linux-x86_64 /opt/cmake && \
    rm cmake-3.25.0-linux-x86_64.tar.gz
ENV PATH="/opt/cmake/bin:${PATH}"

# Install VCPKG
RUN apt-get update && \
    apt-get install -y git build-essential && \
    cd /tmp && \
    wget --no-check-certificate https://github.com/microsoft/vcpkg/archive/refs/tags/2023.02.24.tar.gz && \
    tar -xzvf 2023.02.24.tar.gz && \
    mv vcpkg-2023.02.24 /opt/vcpkg && \
    rm 2023.02.24.tar.gz && \
    cd /opt/vcpkg && \
    ./bootstrap-vcpkg.sh
ENV PATH="/opt/vcpkg:${PATH}"

