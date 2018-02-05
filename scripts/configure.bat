call "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvarsall" x64
echo off

REM ## Set the Location of RE here ##
set RE_PATH=C:/libs/re/


REM ## Set the location of the common directory in which libraries are stored here ##
set LIB_ROOT=C:/libs/


REM ## Set the location of each library path of required middlewares (defaults should work fine assuming common install location) ##
set BOOST_ROOT=%LIB_ROOT%/boost_1_63_0-x64/
set BOOST_COMPILER="-vc140"
set BOOST_INCLUDE_DIRS=%BOOST_ROOT%/boost/
set BOOST_LIBRARYDIR=%BOOST_ROOT%/lib64-msvc-14.0/
set PROTOBUF_ROOT=%LIB_ROOT%/protobuf-3.2.0-x64/
set PROTOBUF_INCLUDE_DIRS=PROTOBUF_ROOT=%/include/
set PUGIXML_ROOT=%LIB_ROOT%/pugixml-1.8-x64/
set ZMQ_ROOT_DIR=%LIB_ROOT%/zeromq-4.2.1-x64/

REM ## Set the location of SIGAR
set SIGAR_ROOT=%LIB_ROOT%/sigar/
set SIGAR_INCLUDE_DIRS=%SIGAR_ROOT%/include/
set SIGAR_LIBRARYDIR=%SIGAR_ROOT%/lib/


REM ## Set OpenCL path details here ##
REM set OPENCL_ROOT=%OCL_ROOT%
REM set OPENCL_LIB_PATH=%OPENCL_ROOT%/lib/x86_64
REM set CLFFT_ROOT=%CLFFT_ROOT%
REM set CLFFT_LIB_ROOT=%CLFFT_ROOT%/lib64/import/

REM ## Set CMAKE enviroment variables along with path ##
REM set CMAKE_INCLUDE_PATH=%ZMQ_ROOT_DIR%include;%PROTOBUF_ROOT%/include;%BOOST_ROOT%/boost;
REM set CMAKE_LIBRARY_PATH=%ZMQ_ROOT_DIR%/lib;%PROTOBUF_ROOT%/lib;
set CMAKE_MODULE_PATH=%RE_PATH%/cmake_modules
set PATH=%PATH%;%PROTOBUF_ROOT%/bin;%ZMQ_ROOT_DIR%/bin;%BOOST_LIBRARYDIR%;%RE_PATH%/bin;%SIGAR_ROOT%/bin;

REM ## Set CMAKE compiler flags ##
set CMAKE_CXX_FLAGS=%CMAKE_CXX_FLAGS% /bigobj