cmake_minimum_required (VERSION 2.6)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(PROJ_NAME precompile_opencl)

set(OPENCL_ROOT $ENV{OPENCL_ROOT})
set(OPENCL_LIB_PATH $ENV{OPENCL_LIB_PATH})
set(OPENCL_INCLUDE_PATH $ENV{OPENCL_INCLUDE_PATH})

project(${PROJ_NAME})

find_library(OPENCL_LIB OpenCL ${OPENCL_LIB_PATH})

if(NOT OPENCL_LIB)
    message(STATUS "Not Found OpenCL - Not building precompile_opencl")
    return()
endif()

if(NOT OPENCL_INCLUDE_PATH)
    set(OPENCL_INCLUDE_PATH "${OPENCL_ROOT}/include")
endif()

find_package(Boost 1.30.0 COMPONENTS filesystem REQUIRED)


set(SOURCE
    ${CMAKE_CURRENT_SOURCE_DIR}/precompiler.cpp
#    ${CMAKE_CURRENT_SOURCE_DIR}/../openclutilities.cpp
    )

set(HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/../openclutilities.h
    )

add_executable(${PROJ_NAME} ${SOURCE} ${HEADERS})

#Include OpenCL Includes
target_include_directories(${PROJ_NAME} PRIVATE ${OPENCL_INCLUDE_PATH})
target_include_directories(${PROJ_NAME} PRIVATE ${Boost_INCLUDE_DIRS})
target_include_directories(${PROJ_NAME} PRIVATE ${RE_SRC_PATH})

if (UNIX)
    target_link_libraries(${PROJ_NAME} pthread)
endif(UNIX)

target_compile_definitions(${PROJ_NAME} PRIVATE -DBOOST_ALL_DYN_LINK)

target_link_libraries(${PROJ_NAME} opencl_worker)
target_link_libraries(${PROJ_NAME} ${OPENCL_LIB})
target_link_libraries(${PROJ_NAME} re_core)
