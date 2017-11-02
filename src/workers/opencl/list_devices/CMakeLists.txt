cmake_minimum_required (VERSION 2.6)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
#Get the RE_PATH
set(RE_PATH "$ENV{RE_PATH}")

set(PROJ_NAME list_opencl_devices)

set(OPENCL_ROOT $ENV{OPENCL_ROOT})
set(OPENCL_LIB_PATH $ENV{OPENCL_LIB_PATH})
#set(CLFFT_ROOT $ENV{CLFFT_ROOT})
#set(CLFFT_LIB_ROOT $ENV{CLFFT_LIB_ROOT})
project(${PROJ_NAME})

find_library(OPENCL_LIB OpenCL ${OPENCL_LIB_PATH})
#find_library(CLFFT_LIB clFFT ${CLFFT_LIB_ROOT})

#link_directories(${CLFFT_ROOT}/lib64)

if(NOT OPENCL_LIB)
    message(STATUS "Not Found OpenCL - Not building GPU device enumerator")
    return()
endif()

#if(NOT CLFFT_LIB)
#    message(STATUS "Not Found CLFFT - Not building GPU device enumerator")
#    return()
#endif(NOT CLFFT_LIB)

set(SOURCE
    ${CMAKE_CURRENT_SOURCE_DIR}/listdevices.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/jsonwriter.cpp
    )

set(HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/jsonwriter.h
#    ${CMAKE_CURRENT_SOURCE_DIR}/../clErrorPrinter.h
    )

#Add RE_PATH to include directories
#include_directories("${RE_PATH}/src")
#include_directories("${CLFFT_ROOT}/include")
include_directories("${OPENCL_ROOT}/include")

#Use this in production
#add_library(${PROJ_NAME} MODULE ${SOURCE} ${HEADERS})

#Use this for enable_testing
#add_library(${PROJ_NAME}_lib ${SOURCE} ${HEADERS})
add_executable(${PROJ_NAME} listdevices.cpp jsonwriter.cpp)

# if (MSVC)
# 	#Visual studio needs to be told to build in Multithreaded Dynamically Linked mode
# 	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MD")
# elseif(UNIX)
# 	target_link_libraries(${PROJ_NAME} pthread)
# endif(MSVC)


#target_link_libraries(${PROJ_NAME} re_core)
target_link_libraries(${PROJ_NAME} ${OPENCL_LIB})
#target_link_libraries(${PROJ_NAME} ${CLFFT_LIB})

#target_link_libraries(${PROJ_NAME}_lib ${OPENCL_LIB})
#target_link_libraries(${PROJ_NAME} ${PROJ_NAME}_lib)

