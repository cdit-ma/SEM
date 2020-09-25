FROM ubuntu:18.04

#Install required build tools
RUN apt-get update && apt-get install -y \
        git \
        cmake \
        build-essential \
        wget \
        ninja-build \
        libpq-dev \
        postgresql-server-dev-all \
        libcurl4-openssl-dev \
        software-properties-common \
    && rm -rf /var/lib/apt/lists/*

# Install latest version of cmake
RUN wget -O - https://apt.kitware.com/keys/kitware-archive-latest.asc 2>/dev/null \
    | gpg --dearmor - | tee /etc/apt/trusted.gpg.d/kitware.gpg >/dev/null && \
    apt-add-repository 'deb https://apt.kitware.com/ubuntu/ bionic main' && \
    apt-get update && \
    apt-get install cmake -y && \
    rm -rf /var/lib/apt/lists/*

#Install Boost
RUN wget https://dl.bintray.com/boostorg/release/1.67.0/source/boost_1_67_0.tar.gz -q -P /opt/ && \
        tar xf /opt/boost_1_67_0.tar.gz -C /opt/ && \
        cd /opt/boost_1_67_0 && \
        ./bootstrap.sh --with-libraries=filesystem,system,iostreams,program_options,thread,regex,date_time,chrono,atomic && \
        ./b2 -j6 -d0 && \
        ./b2 install -d0 && \
        cd /opt && \
        rm -rf /opt/boost*

RUN ldconfig
