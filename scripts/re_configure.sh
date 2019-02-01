#!/usr/bin/env bash
echo "<<< CDIT-MA re Configure >>>"

#Set project locations
export RE_PATH=/home/cdit-ma/re
export CMAKE_MODULE_PATH=$RE_PATH/cmake_modules
export RE_GEN_PATH=$RE_PATH/re_gen
export PUGIXML_PATH=/usr/local

# QPID
export QPID_ROOT=/opt/QpidPb

RE_LIB_PATH=$QPID_ROOT/lib/
LD_LIBRARY_PATH=$RE_LIB_PATH:$LD_LIBRARY_PATH

# Set OpenCL paths
# AMD
#export AMDSDKPATH=/opt/AMDAPPSDK-3.0
#export OPENCL_ROOT=$AMDSDKPATH
#export OPENCL_LIB_PATH=$OPENCL_ROOT/lib/x86_64/sdk

#PATH=$OPENCL_ROOT/include:$PATH
#PATH=$OPENCL_LIB_PATH:$PATH
#LD_LIBRARY_PATH=$OPENCL_LIB_PATH:$LD_LIBRARY_PATH

# clFFT
#export CLFFT_ROOT=/opt/clfft/clFFT-2.10.2-Linux-x64
#export CLFFT_LIB_ROOT=$CLFFT_ROOT/lib64
#PATH=$CLFFT_ROOT/bin:$PATH
#LD_LIBRARY_PATH=$CLFFT_ROOT/lib64:$LD_LIBRARY_PATH

# OSPL
. /opt/HDE/x86_64.linux/release.com

#RTI-Ubuntu
#RTI_DDS_HOST="x64Linux3gcc5.4.0"

#RTI-Centos
#RTI_DDS_HOST="x64Linux3gcc4.8.2"

#RTI-MacOSX
#RTI_DDS_HOST="x64Darwin16clang8.0"

# RTI-ALL
#. /opt/RTI/rti_connext_dds-5.3.0/resource/scripts/rtisetenv_$RTI_DDS_HOST.bash
#export RTI_DDS_HOST

#ACE/TAO - Either
#ACE_ROOT=/opt/ACE_6.5.0/
#TAO_ROOT=$ACE_ROOT/TAO
#LD_LIBRARY_PATH=$ACE_ROOT/lib:$LD_LIBRARY_PATH


export LD_LIBRARY_PATH

