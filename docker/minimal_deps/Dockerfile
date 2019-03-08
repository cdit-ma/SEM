FROM ubuntu:18.04

#Install required build tools
RUN apt-get update && apt-get install -y \
        git \
        cmake \
        build-essential \
        wget \
        ninja-build \
	libpqxx-dev \
    && rm -rf /var/lib/apt/lists/*

#Install Boost
RUN wget https://dl.bintray.com/boostorg/release/1.67.0/source/boost_1_67_0.tar.gz -q -P /opt/ && \
        tar xf /opt/boost_1_67_0.tar.gz -C /opt/ && \
        cd /opt/boost_1_67_0 && \
        ./bootstrap.sh --with-libraries=filesystem,system,iostreams,program_options,thread && \
        ./b2 -j6 -d0 && \
        ./b2 install -d0 && \
        cd /opt && \
        rm -rf /opt/boost*

#Install ZeroMQ
RUN wget https://github.com/zeromq/libzmq/releases/download/v4.2.0/zeromq-4.2.0.tar.gz -q -P /opt/ && \
        tar xf /opt/zeromq-4.2.0.tar.gz -C /opt/ && \
        mkdir /opt/zeromq-4.2.0/build && \
        cd /opt/zeromq-4.2.0/build && \
        cmake -G Ninja .. > /dev/null 2>&1  && \
        cd / && \
        cmake --build /opt/zeromq-4.2.0/build --target install > /dev/null 2>&1 && \
        rm -rf /opt/zeromq* && \
        wget https://raw.githubusercontent.com/zeromq/cppzmq/master/zmq.hpp -q -P /usr/local/include/

#Install Protobuf
RUN wget https://github.com/google/protobuf/releases/download/v3.6.0/protobuf-cpp-3.6.0.tar.gz -q -P /opt/ && \
        tar xf /opt/protobuf-cpp-3.6.0.tar.gz -C /opt/ && \
        mkdir /opt/protobuf-3.6.0/cmake/build && \
        cd /opt/protobuf-3.6.0/cmake/build && \
        cmake -G Ninja -DCMAKE_BUILD_TYPE=Release -Dprotobuf_BUILD_TESTS=OFF -Dprotobuf_BUILD_EXAMPLES=OFF -DCMAKE_POSITION_INDEPENDENT_CODE=ON .. > /dev/null 2>&1 && \
        cmake --build . --target install > /dev/null 2>&1 && \
        cd / && \
        rm /opt/protobuf* -rf

#Install pugixml
RUN wget http://github.com/zeux/pugixml/releases/download/v1.8/pugixml-1.8.tar.gz -q -P /opt/ && \
        tar xf /opt/pugixml-1.8.tar.gz -C /opt/ && \
        mkdir /opt/pugixml-1.8/build && \
        cd /opt/pugixml-1.8/build && \
        cmake -G Ninja .. -DCMAKE_POSITION_INDEPENDENT_CODE=ON > /dev/null 2>&1 && \
        cmake --build . --target install > /dev/null 2>&1 && \
        cd / && \
        rm /opt/pugixml* -rf

#Install sigar
RUN wget https://github.com/cdit-ma/sigar/archive/sigar-1.6.4B.tar.gz -q -P /opt/ && \
        tar xf /opt/sigar-1.6.4B.tar.gz -C /opt/ && \
        mkdir /opt/sigar-sigar-1.6.4B/build && \
        cd /opt/sigar-sigar-1.6.4B/build && \
        cmake -G Ninja .. > /dev/null 2>&1 && \
        cmake --build . --target install > /dev/null 2>&1 && \
        cd / && \
        rm /opt/sigar* -rf

ENV PUGIXML_PATH /usr/local

RUN ldconfig
