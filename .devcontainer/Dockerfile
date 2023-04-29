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
ENV CMAKE_VERSION 3.26.3
RUN apt update && \
    cd /tmp && \
    wget --no-check-certificate https://github.com/Kitware/CMake/releases/download/v${CMAKE_VERSION}/cmake-${CMAKE_VERSION}-linux-x86_64.tar.gz && \
    tar -xzvf cmake-${CMAKE_VERSION}-linux-x86_64.tar.gz && \
    mv cmake-${CMAKE_VERSION}-linux-x86_64 /opt/cmake && \
    rm cmake-${CMAKE_VERSION}-linux-x86_64.tar.gz
ENV PATH="/opt/cmake/bin:${PATH}"

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

# Download Clang from source
#ENV CLANG_VERSION 15.0.7
#RUN apt update && \
#    cd /tmp && \
#    curl -LO https://github.com/llvm/llvm-project/archive/refs/tags/llvmorg-${CLANG_VERSION}.tar.gz && \
#    tar -xvf llvmorg-${CLANG_VERSION}.tar.gz && \
#    rm llvmorg-${CLANG_VERSION}.tar.gz

# Build Clang from source
#RUN apt update && \
#    cd /tmp/llvm-project-llvmorg-${CLANG_VERSION} && \
#    mkdir build && cd build && \
#    cmake \
#        -DLLVM_ENABLE_PROJECTS=clang \
#        -DCMAKE_BUILD_TYPE=Release \
#        -G "Unix Makefiles" \
#        ../llvm \
#        && \
#    make clang -j $(nproc)

# Install Clang from source
#RUN cd /tmp/llvm-project-llvmorg-${CLANG_VERSION} && \
#    mv build /opt/clang-${CLANG_VERSION}
#ENV PATH="/opt/clang-15.0.7/bin:${PATH}"

# Download and install Clang binaries
ENV CLANG_VERSION 16.0.2
RUN cd /tmp && \
    curl -L -O https://github.com/llvm/llvm-project/releases/download/llvmorg-${CLANG_VERSION}/clang+llvm-${CLANG_VERSION}-x86_64-linux-gnu-ubuntu-22.04.tar.xz && \
    tar -xf clang+llvm-${CLANG_VERSION}-x86_64-linux-gnu-ubuntu-22.04.tar.xz && \
    rm      clang+llvm-${CLANG_VERSION}-x86_64-linux-gnu-ubuntu-22.04.tar.xz
    
# Install Clang
RUN cd /tmp && \
    mkdir /opt/clang-${CLANG_VERSION} && \
    mv clang+llvm-${CLANG_VERSION}-x86_64-linux-gnu-ubuntu-22.04/* /opt/clang-${CLANG_VERSION}
ENV PATH="/opt/clang-${CLANG_VERSION}/bin:${PATH}"

# Download and extract the LunarG Vulkan SDK
ENV VULKAN_SDK_VERSION 1.3.243.0
RUN cd /tmp && \
    curl -L -O https://sdk.lunarg.com/sdk/download/${VULKAN_SDK_VERSION}/linux/vulkansdk-linux-x86_64-${VULKAN_SDK_VERSION}.tar.gz && \
    tar zxvf vulkansdk-linux-x86_64-${VULKAN_SDK_VERSION}.tar.gz && \
    rm vulkansdk-linux-x86_64-${VULKAN_SDK_VERSION}.tar.gz && \
    mv ${VULKAN_SDK_VERSION} /opt/vulkansdk-${VULKAN_SDK_VERSION}
    
# Setup Vulkan SDK environment variables
ENV VULKAN_SDK="/opt/vulkansdk-${VULKAN_SDK_VERSION}/x86_64"
ENV PATH="${VULKAN_SDK}/bin:$PATH"
ENV LD_LIBRARY_PATH="${VULKAN_SDK}/lib:$LD_LIBRARY_PATH"
ENV VK_LAYER_PATH="${VULKAN_SDK}/etc/vulkan/explicit_layer.d"

# Set environment variables for NVIDIA and Vulkan
ENV NVIDIA_VISIBLE_DEVICES all
ENV NVIDIA_DRIVER_CAPABILITIES graphics,utility,compute
#ENV VK_ICD_FILENAMES /usr/share/vulkan/icd.d/nvidia_icd.json
ENV VK_ICD_FILENAMES=/vulkan-icd/nvidia_icd.json

RUN ICD_NVIDIA="/vulkan-icd/nvidia_icd.json" && \
    mkdir -p /vulkan-icd && \
    touch /vulkan-icd/nvidia_icd.json \
    && echo '{' >> $ICD_NVIDIA \
    && echo '    "file_format_version" : "1.0.0",' >> $ICD_NVIDIA \
    && echo '    "ICD": {' >> $ICD_NVIDIA \
    && echo '        "library_path": "libGLX_nvidia.so.0",' >> $ICD_NVIDIA \
    && echo '        "api_version" : "1.3.243"' >> $ICD_NVIDIA \
    && echo '    }' >> $ICD_NVIDIA \
    && echo '}' >> /$ICD_NVIDIA

# Install ZSH
RUN sudo apt install -y zsh && \
    sudo chsh -s /usr/bin/zsh && \
    # Install oh my zsh
    sh -c "$(curl -fsSL https://raw.githubusercontent.com/ohmyzsh/ohmyzsh/master/tools/install.sh)" && \
    # configure .zshrc
    cd ~ && \
    touch .zshrc && \
    sed -i 's/ZSH_THEME=.*/ZSH_THEME="eastwood"/' .zshrc && \
    echo "ZSH Installed"

# Install vulkan dependencies
RUN apt-get update && apt-get install -y --install-recommends \
    #other
    nano \
    net-tools \
    # vulkan
    libvulkan1 \
    mesa-vulkan-drivers \
    vulkan-tools

# Install glfw dependencies
RUN apt-get update && apt-get install -y --install-recommends \
    # open gl    
    libgl1-mesa-dev \
    # x general
    libxrandr-dev \
    libxinerama-dev \
    libxcursor-dev \
    xauth \
    libx11-dev \
    libx11-xcb-dev \
    libxi-dev \
    libxcb1-dev \
    libxcb-randr0-dev 

ENTRYPOINT ["/usr/bin/zsh"]

WORKDIR /home/benjamin
