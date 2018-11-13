FROM minimal_deps

ARG webdav_password

RUN apt-get update && apt-get install -y expect sed && rm -rf /var/lib/apt/lists/*
COPY install_rti.exp /opt/install_rti.exp
COPY Makefile.Build_DCPS_ISO_Cpp_Lib /opt/Makefile.Build_DCPS_ISO_Cpp_Lib

RUN wget http://192.168.111.1/raid/software_share/Software/RTI/rti_connext_dds-5.2.3-eval-x64Linux3gcc4.8.2.run --user root --password $webdav_password  -q -P /opt/ && \
        chmod +x /opt/rti_connext_dds-5.2.3-eval-x64Linux3gcc4.8.2.run && \
        chmod +x /opt/install_rti.exp && \
        sync && \
        /opt/install_rti.exp

RUN wget http://192.168.111.1/raid/software_share/Middlewares/DDS/OpenSpliceDDS/6.4/OpenSpliceDDSV6.4.140407OSS-HDE-x86_64.linux-gcc4.6-glibc2.15-installer.tar.gz --user root --password $webdav_password -q -P /opt/ && \
        tar xf /opt/OpenSpliceDDSV6.4.140407OSS-HDE-x86_64.linux-gcc4.6-glibc2.15-installer.tar.gz -C /opt/ && \
        cp /opt/Makefile.Build_DCPS_ISO_Cpp_Lib /opt/HDE/x86_64.linux/custom_lib/Makefile.Build_DCPS_ISO_Cpp_Lib && \
        cd /opt && \
        sed -i "s|@@INSTALLDIR@@|$PWD|g" HDE/x86_64.linux/release.com && \
        cd /opt/HDE/x86_64.linux/custom_lib && \
        make Build_DCPS_ISO_Cpp_Lib && \
        cd /

RUN wget http://apache.mirror.serversaustralia.com.au/qpid/cpp/1.36.0/qpid-cpp-1.36.0.tar.gz --user root --password $webdav_password -q -P /opt/ && \
        tar xf /opt/qpid-cpp-1.36.0.tar.gz -C /opt/ && \
        cd /opt/qpid-cpp-1.36.0 && \
        mkdir build && \
        cd build && \
        cmake .. -DCMAKE_INSTALL_PREFIX=/opt/QpidPb -DPYTHON_EXECUTABLE:FILEPATH=/usr/bin/python -DBUILD_BINDING_PERL=OFF -DBUILD_BINDING_PYTHON=OFF -DBUILD_BINDING_RUBY=OFF && \
        make all && \
        make install && \
        cd /

#Install re
RUN     cd re && \
        mkdir build && \
        cd build && \
        cmake  -G Ninja ..  && \
        cmake --build .  && \
        cd /