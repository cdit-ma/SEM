
apt-get update && DEBIAN_FRONTEND=noninteractive apt-get install -y expect sed python-minimal uuid-dev ruby bison

wget http://192.168.111.1/raid/software_share/Middlewares/DDS/RTI/5.3/rti_connext_dds-5.3.0-eval-x64Linux3gcc5.4.0.run -q && \
    chmod +x rti_connext_dds-5.3.0-eval-x64Linux3gcc5.4.0.run && \
    sudo ./rti_connext_dds-5.3.0-eval-x64Linux3gcc5.4.0.run && \
    rm -rf rti*

sudo wget http://192.168.111.1/raid/software_share/Middlewares/DDS/OpenSpliceDDS/6.4/OpenSpliceDDSV6.4.140407OSS-HDE-x86_64.linux-gcc4.6-glibc2.15-installer.tar.gz -q -P /opt/ && \
    sudo tar xf /opt/OpenSpliceDDSV6.4.140407OSS-HDE-x86_64.linux-gcc4.6-glibc2.15-installer.tar.gz -C /opt/ && \
    sudo cp Makefile.Build_DCPS_ISO_Cpp_Lib /opt/HDE/x86_64.linux/custom_lib/Makefile.Build_DCPS_ISO_Cpp_Lib && \
    cd /opt && \
    sudo sed -i "s|@@INSTALLDIR@@|$PWD|g" HDE/x86_64.linux/release.com && \
    cd /opt/HDE/x86_64.linux/custom_lib && \
    sudo make Build_DCPS_ISO_Cpp_Lib && \
    sudo rm -rf /opt/OpenSplice*

wget http://192.168.111.1/raid/software_share/Middlewares/QpidPb/qpid-cpp-1.38.0.tar.gz -q && \
    wget http://192.168.111.1/raid/software_share/Middlewares/QpidPb/qpid-proton-0.24.0.tar.gz -q && \
    sudo tar xf qpid-proton-0.24.0.tar.gz -C /opt/ && \
    sudo tar xf qpid-cpp-1.38.0.tar.gz -C /opt/ && \
    sudo mkdir /opt/qpid-proton-0.24.0/build && \
    cd /opt/qpid-proton-0.24.0/build && \
    sudo cmake -G Ninja .. -DCMAKE_INSTALL_PREFIX=/opt/QpidPb -DSYSINSTALL_BINDINGS=ON -DPYTHON_EXECUTABLE:FILEPATH=/usr/bin/python2 && \
    sudo cmake --build . --target install && \
    cd / && \
    sudo mkdir /opt/qpid-cpp-1.38.0/build && \
    cd /opt/qpid-cpp-1.38.0/build && \
    sudo cmake -G Ninja .. -DCMAKE_INSTALL_PREFIX=/opt/QpidPb -DPYTHON_EXECUTABLE:FILEPATH=/usr/bin/python -DBUILD_BINDING_PERL=OFF -DBUILD_BINDING_PYTHON=OFF -DBUILD_BINDING_RUBY=OFF && \
    sudo cmake --build . --target install && \
    cd ~ && \
    sudo rm -rf qpid*