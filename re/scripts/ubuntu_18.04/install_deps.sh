#Build Deps
#UBUNTU
apt-get update && \
apt-get install -y \
    apt-transport-https \
    ca-certificates \
    gnupg \
    software-properties-common \
    git \
    cmake \
    build-essential \
    wget \
    ninja-build \
    openjdk-11-jre-headless \
    ccache \
    libpq-dev \
    postgresql-server-dev-all \
    libcurl4-openssl-dev && \

# Install Docker
apt-get install -y \
    apt-transport-https \
    ca-certificates \
    curl \
    libcurl4 \
    libcurl4-openssl-dev \
    gnupg-agent \
    software-properties-common && \
curl -fsSL https://download.docker.com/linux/ubuntu/gpg | apt-key add - && \
add-apt-repository \
    "deb [arch=amd64] https://download.docker.com/linux/ubuntu \
    $(lsb_release -cs) \
    stable" && \
apt-get update && \
apt-get install -y docker-ce docker-ce-cli containerd.io && \


# Install Boost
wget https://dl.bintray.com/boostorg/release/1.67.0/source/boost_1_67_0.tar.gz && \
tar xf boost_1_67_0.tar.gz && \
pushd boost_1_67_0 && \
./bootstrap.sh && \
./b2 -j6 -d0 && \
./b2 install -d0 && \
popd && \
rm -rf boost_1_67_0.tar.gz boost_1_67_0 && \

# Install latest version of cmake
wget -O - https://apt.kitware.com/keys/kitware-archive-latest.asc 2>/dev/null \
    | gpg --dearmor - | tee /etc/apt/trusted.gpg.d/kitware.gpg >/dev/null && \
apt-add-repository 'deb https://apt.kitware.com/ubuntu/ bionic main' && \
apt-get update && \
apt-get install cmake -y && \

# Setup Jenkins Directory
mkdir /opt/Jenkins && \
chown cdit-ma /opt/Jenkins/

# TODO: Warn on missing chrony install?