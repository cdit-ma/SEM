cmake_minimum_required (VERSION 2.6)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
#Get the RE_PATH
set(RE_PATH "$ENV{RE_PATH}")

set(PROJ_NAME opencl_worker)
set(TESTER_NAME ${PROJ_NAME}_tester)

set(OPENCL_ROOT $ENV{OPENCL_ROOT})
set(OPENCL_LIB_PATH $ENV{OPENCL_LIB_PATH})
#set(CLFFT_ROOT $ENV{CLFFT_ROOT})
#set(CLFFT_LIB_ROOT $ENV{CLFFT_LIB_ROOT})
project(${PROJ_NAME})

find_library(OPENCL_LIB OpenCL ${OPENCL_LIB_PATH})
#find_library(CLFFT_LIB clFFT ${CLFFT_LIB_ROOT})

if(NOT OPENCL_LIB)
    message(STATUS "Not Found OpenCL - Not building GPU worker")
    return()
endif()

#if(NOT CLFFT_LIB)
#    message(STATUS "Not Found CLFFT - Not building GPU worker")
#    return()
#endif(NOT CLFFT_LIB)

set(SOURCE
    ${CMAKE_CURRENT_SOURCE_DIR}/openclmanager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/genericbuffer.cpp
    )

set(HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/openclmanager.h
    ${CMAKE_CURRENT_SOURCE_DIR}/openclutilities.h
    ${CMAKE_CURRENT_SOURCE_DIR}/genericbuffer.h
    ${CMAKE_CURRENT_SOURCE_DIR}/oclbuffer.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/rekernel.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/cl2.hpp
    )


#Add RE_PATH to include directories
include_directories("${RE_PATH}/src")
include_directories("${OPENCL_ROOT}/include")
#include_directories("${CLFFT_ROOT}/include")

#Use this for enable_testing
add_library(${PROJ_NAME} SHARED ${SOURCE} ${HEADERS})
add_executable(${TESTER_NAME} openclworkertests.cpp)

if (MSVC)
 	#Visual studio needs to be told to build in Multithreaded Dynamically Linked mode
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MD")
elseif(UNIX)
	target_link_libraries(${PROJ_NAME} pthread)
endif(MSVC)


target_link_libraries(${PROJ_NAME} re_core)
target_link_libraries(${PROJ_NAME} ${OPENCL_LIB})
target_link_libraries(${PROJ_NAME} ${CLFFT_LIB})

target_link_libraries(${TESTER_NAME} re_core)
target_link_libraries(${TESTER_NAME} ${PROJ_NAME})
target_link_libraries(${TESTER_NAME} ${OPENCL_LIB})
#target_link_libraries(${TESTER_NAME} ${CLFFT_LIB})
