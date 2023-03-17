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
    cmake \
        -DLLVM_ENABLE_PROJECTS=clang \
        -DCMAKE_BUILD_TYPE=Release \
        -G "Unix Makefiles" \
        ../llvm \
        && \
    make clang -j $(nproc)

# Install Clang 15.0.7
RUN cd /tmp/llvm-project-llvmorg-15.0.7 && \
    mv build /opt/clang-15.0.7
ENV PATH="/opt/clang-15.0.7/bin:${PATH}"

# Download and extract the LunarG Vulkan SDK 1.3.239
ENV VULKAN_SDK_VERSION 1.3.239.0
RUN cd /tmp && \
    wget https://sdk.lunarg.com/sdk/download/${VULKAN_SDK_VERSION}/linux/vulkansdk-linux-x86_64-${VULKAN_SDK_VERSION}.tar.gz && \
    tar zxvf vulkansdk-linux-x86_64-${VULKAN_SDK_VERSION}.tar.gz && \
    rm vulkansdk-linux-x86_64-${VULKAN_SDK_VERSION}.tar.gz && \
    mv 1.3.239.0 /opt/vulkansdk-${VULKAN_SDK_VERSION}
    
# Setup Vulkan SDK environment variables
ENV VULKAN_SDK="/opt/vulkansdk-${VULKAN_SDK_VERSION}/x86_64"
ENV PATH="${VULKAN_SDK}/bin:$PATH"
ENV LD_LIBRARY_PATH="${VULKAN_SDK}/lib:$LD_LIBRARY_PATH"
ENV VK_LAYER_PATH="${VULKAN_SDK}/etc/vulkan/explicit_layer.d"

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

#Install SSH

# Set environment variables for NVIDIA and Vulkan
ENV NVIDIA_VISIBLE_DEVICES all
ENV NVIDIA_DRIVER_CAPABILITIES graphics,utility,compute
ENV VK_ICD_FILENAMES /usr/share/vulkan/icd.d/nvidia_icd.json
ENV VK_ICD_FILENAMES=/vulkan-icd/nvidia_icd.json

RUN ICD_NVIDIA="/vulkan-icd/nvidia_icd.json" && \
    mkdir -p /vulkan-icd && \
    touch /vulkan-icd/nvidia_icd.json && \
    echo '{' >> $ICD_NVIDIA \
    && echo '    "file_format_version" : "1.0.0",' >> $ICD_NVIDIA \
    && echo '    "ICD": {' >> $ICD_NVIDIA \
    && echo '        "library_path": "libGLX_nvidia.so.0",' >> $ICD_NVIDIA \
    && echo '        "api_version" : "1.3.239"' >> $ICD_NVIDIA \
    && echo '    }' >> $ICD_NVIDIA \
    && echo '}' >> /$ICD_NVIDIA

# Install necessary dependencies
RUN apt-get update && apt-get install -y --install-recommends \
    nano \
    libvulkan1 \
    mesa-vulkan-drivers \
    vulkan-tools \
    libxcb1-dev \
    libxcb-keysyms1-dev \
    libxcb-dri3-dev \
    libxcb-xinerama0-dev \
    libxcb-icccm4-dev \
    libxcb-image0-dev \
    libxcb-randr0-dev \
    libxcb-shape0-dev \
    libxcb-xkb-dev \
    libxkbcommon-dev \
    libxkbcommon-x11-dev \
    libwayland-dev \
    libx11-xcb-dev \
    libx11-dev \
    libxcb-glx0 \
    libxcb-xinput0 \
    libxcb-render-util0 \
    libegl1 \
    libxcomposite1 \
    libfontconfig1 \
    libgl1 \
    libgl1-mesa-dev \
    libfontconfig1 \
    libxi6 \
    libxi-dev \
    libxrender1 \
    libxrandr-dev \
    libxinerama-dev \
    libxrandr2 \
    libxcursor1 \
    libxcursor-dev \
    libdbus-1-3 \
    xauth

# change root password
#RUN echo 'root:password' | chpasswd

#RUN apt-get update && \
#    apt-get install -y \
#    openssh-server \
#    systemctl

# Configure SSH
#RUN mkdir /var/run/sshd
#RUN sed -i 's/#PermitRootLogin prohibit-password/PermitRootLogin yes/' /etc/ssh/sshd_config

# Start SSH
#EXPOSE 22

#sudo apt-get install libnvidia-egl-wayland1 libnvidia-gl-525


ENTRYPOINT ["/usr/bin/zsh"]

#RUN useradd -u 1000 benjamin && \
#    echo "benjamin:benjamin" | chpasswd && \
#    adduser benjamin sudo && \
#    mkdir -p /home/benjamin && \
#    chown -R benjamin /home/benjamin && \
#    printf "\benjamin ALL=(ALL) NOPASSWD: ALL" >> /etc/sudoers
#USER benjamin
#WORKDIR /home/benjamin