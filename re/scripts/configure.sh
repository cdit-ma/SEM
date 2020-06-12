#!/usr/bin/env bash

#Get the location of the script.
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
RE_PATH=$SCRIPT_DIR/../
RE_LIB_PATH=$RE_PATH/lib/
CMAKE_MODULE_PATH=$RE_PATH/cmake_modules/

PUGIXML_ROOT="/opt/pugixml-1.8/"

RTI_DDS_HOST="x64Darwin16clang8.0"
export RTI_DDS_HOST

CLFFT_ROOT=/opt/clfft/clFFT-2.10.2/src
CLFFT_LIB_ROOT=$CLFFT_ROOT/library

#Set library path to include the re_core location
LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:$RE_LIB_PATH:$CLFFT_ROOT/library
DYLD_FALLBACK_LIBRARY_PATH=${DYLD_FALLBACK_LIBRARY_PATH}:$RE_LIB_PATH:/usr/lib:/usr/local/lib:$CLFFT_ROOT/library

export RE_PATH PUGIXML_ROOT LD_LIBRARY_PATH DYLD_FALLBACK_LIBRARY_PATH CMAKE_MODULE_PATH CLFFT_ROOT CLFFT_LIB_ROOT
echo "<<< cdit-ma runtime environment configure script >>>"
echo "RE_PATH: $RE_PATH"