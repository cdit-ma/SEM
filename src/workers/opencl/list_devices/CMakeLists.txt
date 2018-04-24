cmake_minimum_required (VERSION 2.6)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


set(PROJ_NAME list_opencl_devices)

set(OPENCL_ROOT $ENV{OPENCL_ROOT})
set(OPENCL_INCLUDE_PATH $ENV{OPENCL_INCLUDE_PATH})
set(OPENCL_LIB_PATH $ENV{OPENCL_LIB_PATH})

project(${PROJ_NAME})

find_library(OPENCL_LIB OpenCL ${OPENCL_LIB_PATH})

if(NOT OPENCL_LIB)
    message(STATUS "Not Found OpenCL - Not building list_opencl_devices")
    return()
endif()

if(NOT OPENCL_INCLUDE_PATH)
    SET(OPENCL_INCLUDE_PATH "${OPENCL_ROOT}/include")
endif()

set(SOURCE
    ${CMAKE_CURRENT_SOURCE_DIR}/listdevices.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/jsonwriter.cpp
    )

set(HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/jsonwriter.h
    )


add_executable(${PROJ_NAME} ${SOURCE} ${HEADERS})

#Include OpenCL Includes
target_include_directories(${PROJ_NAME} PRIVATE ${OPENCL_INCLUDE_PATH})

if (UNIX)
    target_link_libraries(${PROJ_NAME} pthread)
endif(UNIX)

target_link_libraries(${PROJ_NAME} ${OPENCL_LIB})
