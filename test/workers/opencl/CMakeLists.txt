#Set our project names
set(PROJ_NAME test_openclworker)
project(${PROJ_NAME})

set(OPENCL_ROOT $ENV{OPENCL_ROOT})
set(OPENCL_LIB_PATH $ENV{OPENCL_LIB_PATH})
set(OPENCL_INCLUDE_PATH $ENV{OPENCL_INCLUDE_PATH})

find_library(OPENCL_LIB OpenCL ${OPENCL_LIB_PATH})

if(NOT OPENCL_LIB)
    message(STATUS "Not Found OpenCL - Not building test_openclworker")
    return()
endif()

if (NOT OPENCL_INCLUDE_PATH)
    set(OPENCL_INCLUDE_PATH "${OPENCL_ROOT}/include")
endif()

SET(SOURCE
    ${CMAKE_CURRENT_SOURCE_DIR}/openclmanagertest.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/openclworker_matrixmult.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/openclworker_runparallel.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/openclworker_buffer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/openclworker_kmeans.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/common.cpp
    ${RE_TEST_SOURCE_DIR}/core/test_main.cpp
)

add_executable(${PROJ_NAME} ${SOURCE})

#Include OpenCL Includes
target_include_directories(${PROJ_NAME} PRIVATE ${OPENCL_INCLUDE_PATH})
target_include_directories(${PROJ_NAME} PRIVATE ${GOOGLE_TEST_DIR}/include)
target_include_directories(${PROJ_NAME} PRIVATE ${RE_SRC_PATH})

if(UNIX)
    target_link_libraries(${PROJ_NAME} pthread)
endif(UNIX)

target_link_libraries(${PROJ_NAME} opencl_worker)
target_link_libraries(${PROJ_NAME} gtest)