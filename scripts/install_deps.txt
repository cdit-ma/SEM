#Build Deps
#UBUNTU
apt-get update && apt-get install -y \
    git \
    cmake \
    build-essential \
    wget \
    ninja-build \
    openjdk-11-jre-headless

#Build Deps
#CENTOS
sudo yum install epel-release git wget java-1.8.0-openjdk ninja-build cmake3
sudo ln -s /usr/bin/cmake3 /usr/bin/cmake

#Install Boost
wget https://dl.bintray.com/boostorg/release/1.67.0/source/boost_1_67_0.tar.gz -q && \
    tar xf boost_1_67_0.tar.gz && \
    cd boost_1_67_0 && \
    ./bootstrap.sh --with-libraries=filesystem,system,iostreams,program_options,thread && \
    ./b2 -j6 -d0 && \
    sudo ./b2 install -d0 && \
    cd ~ && \
    rm -rf boost_1_67_0.tar.gz boost_1_67_0

#Install ZeroMQ
wget https://github.com/zeromq/libzmq/releases/download/v4.2.0/zeromq-4.2.0.tar.gz -q && \
    tar xf zeromq-4.2.0.tar.gz && \
    mkdir zeromq-4.2.0/build && \
    cd zeromq-4.2.0/build && \
    cmake -G Ninja .. && \
    sudo cmake --build . --target install && \
    cd ~ && \
    rm -rf zeromq-4.2.0.tar.gz zeromq-4.2.0 && \
    sudo wget https://raw.githubusercontent.com/zeromq/cppzmq/master/zmq.hpp -q -P /usr/local/include/

#Install Protobuf
wget https://github.com/google/protobuf/releases/download/v3.6.0/protobuf-cpp-3.6.0.tar.gz -q && \
    tar xf protobuf-cpp-3.6.0.tar.gz && \
    mkdir protobuf-3.6.0/cmake/build && \
    cd protobuf-3.6.0/cmake/build && \
    cmake -G Ninja -DCMAKE_BUILD_TYPE=Release -Dprotobuf_BUILD_TESTS=OFF -Dprotobuf_BUILD_EXAMPLES=OFF -DCMAKE_POSITION_INDEPENDENT_CODE=ON .. && \
    sudo cmake --build . --target install && \
    cd ~ && \
    rm protobuf-cpp-3.6.0.tar.gz protobuf-3.6.0 -rf

#Install pugixml
#may need to recursively touch files in extracted pugi directory on centos("find  -type f  -exec touch {} +")
wget http://github.com/zeux/pugixml/releases/download/v1.8/pugixml-1.8.tar.gz -q && \
    tar xf pugixml-1.8.tar.gz && \
    mkdir pugixml-1.8/build && \
    cd pugixml-1.8/build && \
    cmake -G Ninja .. -DCMAKE_POSITION_INDEPENDENT_CODE=ON && \
    sudo cmake --build . --target install && \
    cd ~ && \
    rm pugixml-1.8.tar.gz pugixml-1.8 -rf

#Install sigar
wget https://github.com/cdit-ma/sigar/archive/sigar-1.6.4B.tar.gz -q && \
    tar xf sigar-1.6.4B.tar.gz && \
    mkdir sigar-sigar-1.6.4B/build && \
    cd sigar-sigar-1.6.4B/build && \
    cmake -G Ninja .. && \
    sudo cmake --build . --target install && \
    cd ~ && \
    rm sigar-1.6.4B.tar.gz sigar-sigar-1.6.4B -rf
