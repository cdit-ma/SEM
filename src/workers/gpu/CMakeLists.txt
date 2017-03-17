cmake_minimum_required (VERSION 2.6)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
#Get the RE_PATH
set(RE_PATH "$ENV{RE_PATH}")

set(PROJ_NAME worker_we_gpu)
set(TESTER_NAME ${PROJ_NAME}_tester)

set(CLFFT_ROOT /opt/clfft/clFFT-2.10.2-Linux-x64)
set(AMDSDKPATH /opt/AMDAPPSDK-3.0)
set(OPENCL_ROOT ${AMDSDKPATH})
set(OPENCL_LIB_ROOT ${OPENCL_ROOT}/lib/x86_64/sdk)

project(${PROJ_NAME})

message(${OPENCL_ROOT})
find_library(OPENCL_LIB OpenCL ${OPENCL_LIB_ROOT})
find_library(CLFFT_LIB clFFT ${CLFFT_ROOT}/lib64)

set(SOURCE
    ${CMAKE_CURRENT_SOURCE_DIR}/gpu_worker.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/gpu_worker_impl.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/BufferHashtable.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/MatrixTester.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/FFTTester.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Tester.cpp
    )

set(HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/gpu_worker.h
    ${CMAKE_CURRENT_SOURCE_DIR}/gpu_worker_impl.h
    ${CMAKE_CURRENT_SOURCE_DIR}/BufferHashtable.h
    ${CMAKE_CURRENT_SOURCE_DIR}/BufferItem.h
    ${CMAKE_CURRENT_SOURCE_DIR}/clErrorPrinter.h
    )

#Add RE_PATH to include directories
include_directories("${RE_PATH}/src")
include_directories("${CLFFT_ROOT}/include")
include_directories("${OPENCL_ROOT}/include")

#Use this in production
#add_library(${PROJ_NAME} MODULE ${SOURCE} ${HEADERS})

#Use this for enable_testing
add_library(${PROJ_NAME} SHARED ${SOURCE} ${HEADERS})
add_executable(${TESTER_NAME} gpu_worker_test.cpp)

if (MSVC)
 	#Visual studio needs to be told to build in Multithreaded Dynamically Linked mode
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MD")
elseif(UNIX)
	target_link_libraries(${PROJ_NAME} pthread)
endif(MSVC)


target_link_libraries(${PROJ_NAME} re_core)
target_link_libraries(${PROJ_NAME} ${OPENCL_LIB})
target_link_libraries(${PROJ_NAME} ${CLFFT_LIB})


message(${CLFFT_LIB})
message(OPENCL_LIB: ${OPENCL_LIB})

target_link_libraries(${TESTER_NAME} ${PROJ_NAME})
target_link_libraries(${TESTER_NAME} ${OPENCL_LIB})
target_link_libraries(${TESTER_NAME} ${CLFFT_LIB})
