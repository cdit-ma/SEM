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

add_executable(${PROJ_NAME} "")
target_compile_features(${PROJ_NAME} PRIVATE cxx_std_11)

# Suppress ignored cl_int alignment attribute warning in opencl library
target_compile_options(${PROJ_NAME} PRIVATE -Wno-ignored-attributes)

target_sources(${PROJ_NAME} PRIVATE
    # Sources
    ${CMAKE_CURRENT_SOURCE_DIR}/openclmanagertest.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/openclworker_matrixmult.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/openclworker_runparallel.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/openclworker_buffer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/openclworker_kmeans.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/openclfftworker.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/common.cpp
)

target_link_libraries(${PROJ_NAME} PRIVATE opencl_worker)
target_link_libraries(${PROJ_NAME} PRIVATE re_loggers)
target_link_libraries(${PROJ_NAME} PRIVATE gtest)
target_link_libraries(${PROJ_NAME} PRIVATE gtest_main)
