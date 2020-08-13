#Set our project names
set(PROJ_NAME test_openclworker)
project(${PROJ_NAME} VERSION ${PROJECT_VERSION})

set(OPENCL_ROOT $ENV{OPENCL_ROOT})
set(OPENCL_LIB_PATH $ENV{OPENCL_LIB_PATH})
set(OPENCL_INCLUDE_PATH $ENV{OPENCL_INCLUDE_PATH})

find_library(OPENCL_LIB OpenCL ${OPENCL_LIB_PATH})

if(NOT OPENCL_LIB)
    message(STATUS "Not Found OpenCL - Not building test_openclworker")
    return()
endif()

add_executable(${PROJ_NAME} "")
target_compile_features(${PROJ_NAME} PRIVATE cxx_std_17)

# Suppress ignored cl_int alignment attribute warning in opencl library
target_compile_options(${PROJ_NAME} PRIVATE -Wno-ignored-attributes)

target_sources(${PROJ_NAME} PRIVATE
    # Sources
    ${CMAKE_CURRENT_SOURCE_DIR}/openclmanagertest.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/openclworker_matrixmult.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/openclworker_runparallel.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/openclworker_buffer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/openclworker_kmeans.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/common.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/common.h
)

# Search for OpenCL FFT libraries
set(CLFFT_ROOT $ENV{CLFFT_ROOT})
set(CLFFT_LIB_ROOT $ENV{CLFFT_LIB_ROOT})

find_library(CLFFT_LIB clFFT ${CLFFT_LIB_ROOT})

if(DEFINED ENV{ALTERA_OCLSDK_LIB})
    set(ALTERA_OCLSDK_LIB_ROOT $ENV{ALTERA_OCLSDK_LIB_ROOT})
    find_library(ALTERA_OCLSDK_LIB OpenCL ${ALTERA_OCLSDK_LIB_ROOT})
endif()

if (CLFFT_LIB)
    SET(OPENCL_HAS_FFT true)
elseif (ALTERA_OCLSDK_LIB)
    SET(OPENCL_HAS_FFT true)
else()
    SET(OPENCL_HAS_FFT false)
endif()

if (OPENCL_HAS_FFT)
    target_sources(${PROJECT_NAME} PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/openclfftworker.cpp)
else()
    message("No OpenCL FFT library present, skipping FFT tests")
endif(OPENCL_HAS_FFT)

target_include_directories(${PROJ_NAME} PRIVATE ${RE_TEST_SOURCE_DIR})
#target_include_directories(${PROJ_NAME} PRIVATE ${GOOGLE_TEST_DIR}/include)

target_link_libraries(${PROJ_NAME}
        PRIVATE
        opencl_worker
        re_loggers
        gtest
        gtest_main
        )
