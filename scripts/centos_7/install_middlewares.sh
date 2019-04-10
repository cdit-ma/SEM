# Build Deps
sudo yum install libuuid-devel bison sed ruby bzip2 perl-Data-Dumper

# Use gcc/g++ 7
. /opt/rh/devtools-7/enable

# RTI
#When asked for install location, specify /opt/RTI
wget http://192.168.111.1/raid/software_share/Middlewares/DDS/RTI/5.3/rti_connext_dds-5.3.0-eval-x64Linux3gcc4.8.2.run -q && \
    sudo mkdir /opt/RTI && \
    chmod +x rti_connext_dds-5.3.0-eval-x64Linux3gcc4.8.2.run && \
    sudo ./rti_connext_dds-5.3.0-eval-x64Linux3gcc4.8.2.run && \
    rm -rf rti* && \
    cd /opt/RTI/rti_connext_dds-5.3.0 && \
    sudo wget http://192.168.111.1/raid/software_share/Middlewares/DDS/RTI/rti_license.dat -q && \
    cd ~

# OpenSplice
wget http://192.168.111.1/raid/software_share/Middlewares/DDS/OpenSpliceDDS/6.4/OpenSpliceDDSV6.4.140407OSS-HDE-x86_64.linux-gcc4.6-glibc2.15-installer.tar.gz -q && \
    sudo tar xf OpenSpliceDDSV6.4.140407OSS-HDE-x86_64.linux-gcc4.6-glibc2.15-installer.tar.gz -C /opt/ && \
    sudo wget http://192.168.111.1/raid/software_share/Middlewares/DDS/OpenSpliceDDS/6.4/Makefile.Build_DCPS_ISO_Cpp_Lib \
    -q -O /opt/HDE/x86_64.linux/custom_lib/Makefile.Build_DCPS_ISO_Cpp_Lib && \
    cd /opt && \
    sudo sed -i "s|@@INSTALLDIR@@|$PWD|g" HDE/x86_64.linux/release.com && \
    cd /opt/HDE/x86_64.linux/custom_lib && \
    sudo make Build_DCPS_ISO_Cpp_Lib && \
    cd ~ && \
    sudo rm -rf OpenSplice*

# QpidPb
wget http://192.168.111.1/raid/software_share/Middlewares/QpidPb/qpid-cpp-1.38.0.tar.gz -q && \
    wget http://192.168.111.1/raid/software_share/Middlewares/QpidPb/qpid-proton-0.24.0.tar.gz -q && \
    tar xf qpid-proton-0.24.0.tar.gz && \
    tar xf qpid-cpp-1.38.0.tar.gz && \
    mkdir qpid-proton-0.24.0/build && \
    cd qpid-proton-0.24.0/build && \
    cmake -G Ninja .. -DCMAKE_INSTALL_PREFIX=/opt/QpidPb -DSYSINSTALL_BINDINGS=ON -DPYTHON_EXECUTABLE:FILEPATH=/usr/bin/python2 && \
    sudo cmake --build . --target install && \
    cd && \
    mkdir qpid-cpp-1.38.0/build && \
    cd qpid-cpp-1.38.0/build && \
    cmake -G Ninja .. -DCMAKE_INSTALL_PREFIX=/opt/QpidPb -DPYTHON_EXECUTABLE:FILEPATH=/usr/bin/python -DBUILD_BINDING_PERL=OFF -DBUILD_BINDING_PYTHON=OFF -DBUILD_BINDING_RUBY=OFF && \
    sudo cmake --build . --target install && \
    cd ~ && \
    sudo rm -rf qpid*

# ACE_TAO
sudo ln -s /lib64/librt.so.1 /usr/lib/librt.so && \
    wget http://192.168.111.1/raid/software_share/Middlewares/ACE+TAO/ACE+TAO-6.5.0.tar.bz2 -q && \
    tar -xf ACE+TAO-6.5.0.tar.bz2 && \
    cd ACE_wrappers && \
    export ACE_ROOT=$PWD && \
    export TAO_ROOT=$ACE_ROOT/TAO && \
    export LD_LIBRARY_PATH=$ACE_ROOT/lib:$LD_LIBRARY_PATH && \
    echo -e "#include \"ace/config-linux.h\"" > $ACE_ROOT/ace/config.h && \
    echo -e "include \$(ACE_ROOT)/include/makeinclude/platform_linux.GNU" > $ACE_ROOT/include/makeinclude/platform_macros.GNU && \
    find . -name "GNUmakefile*" -delete && \
    cd $TAO_ROOT && \
    $ACE_ROOT/bin/mwc.pl TAO_ACE.mwc -type gnuace && \
    make CXXFLAGS="-fpermissive" CFLAGS="-fpermissive" -j8 && \
    sudo mv /home/cdit-ma/ACE_wrappers /opt/ACE_6.5.0/ && \
    cd ~ && \
    rm -rf ACE*

# OpenCL
wget http://192.168.111.1/raid/software_share/Drivers/OpenCL/AMD-APP-SDKInstaller-v3.0.130.136-GA-linux64.tar.bz2 && \
    tar xf AMD-APP-SDKInstaller-v3.0.130.136-GA-linux64.tar.bz2 && \
    sudo ./AMD-APP-SDK-v3.0.130.136-GA-linux64.sh && \
    rm -rf AMD*

# CLFFT
wget https://github.com/clMathLibraries/clFFT/releases/download/v2.10.2/clFFT-2.10.2-Linux-x64.tar.gz && \
    sudo mkdir /opt/clfft && \
    sudo tar xf -C /opt/clfft && \
    rm -rf clFFT*
