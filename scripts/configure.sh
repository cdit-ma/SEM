#!/usr/bin/env bash
echo "<<< CDIT-MA re Configure >>>"

#Get the location of the script.
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
RE_PATH=$SCRIPT_DIR/../
RE_LIB_PATH=$RE_PATH/lib/


export AMDSDKPATH=/opt/AMDAPPSDK-3.0
export OPENCL_PATH=$AMDSDKPATH
export OPENCL_LIB_PATH=$OPENCL_PATH/lib/x86_64/sdk

#PATH=$OPENCL_PATH/include:$PATH
#PATH=$OPENCL_LIB_PATH:$PATH
LD_LIBRARY_PATH=$OPENCL_LIB_PATH:$LD_LIBRARY_PATH


export CLFFT_ROOT=/opt/clfft/clFFT-2.10.2-Linux-x64
LD_LIBRARY_PATH=$CLFFT_ROOT/lib64:$LD_LIBRARY_PATH

if [ "$(uname)" == "Darwin" ]; then
    PUGIXML_ROOT="/Users/dan/pugixml-1.8/"
elif [ "$(expr substr $(uname -s) 1 5)" == "Linux" ]; then
    PUGIXML_ROOT="/opt/pugixml-1.8/"
fi

echo "Library Path: $RE_LIB_PATH"
echo "PugiXML Path: $PUGIXML_ROOT"

#Set library path to include the re_core location
LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:$RE_LIB_PATH
DYLD_FALLBACK_LIBRARY_PATH=${DYLD_FALLBACK_LIBRARY_PATH}:$RE_LIB_PATH
export RE_PATH PUGIXML_ROOT LD_LIBRARY_PATH DYLD_FALLBACK_LIBRARY_PATH
