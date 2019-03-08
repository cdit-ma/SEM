FROM minimal_deps

RUN apt-get update && DEBIAN_FRONTEND=noninteractive apt-get install -y expect sed python-minimal uuid-dev ruby bison && rm -rf /var/lib/apt/lists/*
COPY docker/full_deps/install_rti.exp /opt/install_rti.exp
COPY docker/full_deps/Makefile.Build_DCPS_ISO_Cpp_Lib /opt/Makefile.Build_DCPS_ISO_Cpp_Lib

RUN wget http://192.168.111.1/raid/software_share/Middlewares/DDS/RTI/5.3/rti_connext_dds-5.3.0-eval-x64Linux3gcc5.4.0.run -q -P /opt/ && \
        chmod +x /opt/rti_connext_dds-5.3.0-eval-x64Linux3gcc5.4.0.run && \
        chmod +x /opt/install_rti.exp && \
        sync && \
        /opt/install_rti.exp && \
	wget http://192.168.111.1/raid/software_share/Middlewares/DDS/RTI/rti_license.dat -q -P /opt/RTI/rti_connext_dds-5.3.0/ && \
        rm -rf /opt/rti* /opt/install_rti.exp

RUN wget http://192.168.111.1/raid/software_share/Middlewares/DDS/OpenSpliceDDS/6.4/OpenSpliceDDSV6.4.140407OSS-HDE-x86_64.linux-gcc4.6-glibc2.15-installer.tar.gz -q -P /opt/ && \
        tar xf /opt/OpenSpliceDDSV6.4.140407OSS-HDE-x86_64.linux-gcc4.6-glibc2.15-installer.tar.gz -C /opt/ && \
        cp /opt/Makefile.Build_DCPS_ISO_Cpp_Lib /opt/HDE/x86_64.linux/custom_lib/Makefile.Build_DCPS_ISO_Cpp_Lib && \
        cd /opt && \
        sed -i "s|@@INSTALLDIR@@|$PWD|g" HDE/x86_64.linux/release.com && \
        cd /opt/HDE/x86_64.linux/custom_lib && \
        make Build_DCPS_ISO_Cpp_Lib && \
        rm -rf /opt/OpenSplice* /opt/Makefile*

RUN wget http://192.168.111.1/raid/software_share/Middlewares/QpidPb/qpid-cpp-1.38.0.tar.gz -q -P /opt/ && \
        wget http://192.168.111.1/raid/software_share/Middlewares/QpidPb/qpid-proton-0.24.0.tar.gz -q -P /opt/ && \
        tar xf /opt/qpid-proton-0.24.0.tar.gz -C /opt/ && \
        tar xf /opt/qpid-cpp-1.38.0.tar.gz -C /opt/ && \
        mkdir /opt/qpid-proton-0.24.0/build && \
        cd /opt/qpid-proton-0.24.0/build && \
        cmake -G Ninja .. -DCMAKE_INSTALL_PREFIX=/opt/QpidPb -DSYSINSTALL_BINDINGS=ON -DPYTHON_EXECUTABLE:FILEPATH=/usr/bin/python2 && \
        cmake --build . --target install && \
        cd / && \
        mkdir /opt/qpid-cpp-1.38.0/build && \
        cd /opt/qpid-cpp-1.38.0/build && \
        cmake -G Ninja .. -DCMAKE_INSTALL_PREFIX=/opt/QpidPb -DPYTHON_EXECUTABLE:FILEPATH=/usr/bin/python -DBUILD_BINDING_PERL=OFF -DBUILD_BINDING_PYTHON=OFF -DBUILD_BINDING_RUBY=OFF && \
        cmake --build . --target install && \
        cd / && \
        rm -rf /opt/qpid*

ENV QPID_ROOT=/opt/QpidPb
ENV RTI_DDS_HOST=x64Linux3gcc5.4.0
