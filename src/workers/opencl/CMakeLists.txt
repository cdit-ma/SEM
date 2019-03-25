
set(PROJ_NAME opencl_worker)
project(${PROJ_NAME})

# Set OpenCL Root / Lib Path
set(OPENCL_ROOT $ENV{OPENCL_ROOT})
set(OPENCL_LIB_PATH $ENV{OPENCL_LIB_PATH})

# Try and find an OpenCL Implementation
find_library(OPENCL_LIB OpenCL ${OPENCL_LIB_PATH})

if(DEFINED ENV{ALTERA_OCLSDK_LIB})
    set(ALTERA_OCLSDK_LIB_ROOT $ENV{ALTERA_OCLSDK_LIB_ROOT})
    find_library(ALTERA_OCLSDK_LIB OpenCL ${ALTERA_OCLSDK_LIB_ROOT})
endif()

# If OpenCL doesn't exist on this machine we can bail out
if(NOT OPENCL_LIB)
    message(STATUS "Not Found OpenCL - Not building opencl_worker")
    return()
endif()

# Apply overrides to OpenCL include directory if specified
set(OPENCL_INCLUDE_PATH $ENV{OPENCL_INCLUDE_PATH})
if (NOT OPENCL_INCLUDE_PATH)
    message(STATUS "Setting opencl include directory to default (${OPENCL_ROOT}/include)")
    set(OPENCL_INCLUDE_PATH "${OPENCL_ROOT}/include")
endif()

add_library(${PROJ_NAME} SHARED "")

target_sources(${PROJ_NAME} PRIVATE
    # Sources
    ${CMAKE_CURRENT_SOURCE_DIR}/openclutilities.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/openclmanager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/genericbuffer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/openclbufferbase.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/openclkernel.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/openclkernelbase.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/opencl_worker.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/openclloadbalancer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/opencldevice.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/openclexception.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/openclqueue.cpp

    # Headers
    ${CMAKE_CURRENT_SOURCE_DIR}/cl2.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/openclutilities.h
    ${CMAKE_CURRENT_SOURCE_DIR}/openclmanager.h
    ${CMAKE_CURRENT_SOURCE_DIR}/genericbuffer.h
    ${CMAKE_CURRENT_SOURCE_DIR}/openclbuffer.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/openclbufferbase.h
    ${CMAKE_CURRENT_SOURCE_DIR}/openclkernelbase.h
    ${CMAKE_CURRENT_SOURCE_DIR}/openclkernel.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/opencl_worker.h
    ${CMAKE_CURRENT_SOURCE_DIR}/openclloadbalancer.h
    ${CMAKE_CURRENT_SOURCE_DIR}/opencldevice.h
    ${CMAKE_CURRENT_SOURCE_DIR}/openclexception.h
    ${CMAKE_CURRENT_SOURCE_DIR}/opencldevice.cpp
)

# Search for OpenCL FFT libraries
set(CLFFT_ROOT $ENV{CLFFT_ROOT})
set(CLFFT_LIB_ROOT $ENV{CLFFT_LIB_ROOT})

find_library(CLFFT_LIB clFFT ${CLFFT_LIB_ROOT})

# Add implementations for fft library as necessary
if(ALTERA_OCLSDK_LIB)
    message(STATUS "building OpenCL worker with FPGA FFT library")
    target_sources(${PROJ_NAME} PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/fft1d/opencl_worker_alterafpga.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/fft1d/opencl.cpp
    )
elseif (CLFFT_LIB)
    message(STATUS "building OpenCL worker with GPU FFT library (clFFT)")
    target_sources(${PROJ_NAME} PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/opencl_worker_clfft.cpp
    )
else()
    message(STATUS "No FFT library found when building OpenCL worker")
    target_sources(${PROJ_NAME} PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/opencl_worker_fftdummy.cpp
    )
endif()


# Check for whether FFT implementation should use GPU or FPGA (preference FPGA if avaialable)
if(ALTERA_OCLSDK_LIB)
    message(STATUS "Building OpenCL worker with FPGA FFT library")
    target_compile_definitions(${PROJ_NAME} PUBLIC FFT_LIBRARY='2')
elseif(CLFFT_LIB)
    target_compile_definitions(${PROJ_NAME} PUBLIC FFT_LIBRARY='1')
    target_include_directories(${PROJ_NAME} PUBLIC "${CLFFT_ROOT}/include")
    target_link_libraries(${PROJ_NAME} PUBLIC ${CLFFT_LIB})
else()
    message(STATUS "No FFT library found when building OpenCL worker")
endif()

# Specify the location of kernels - consider moving kernels to be exported alongside binaries?
get_filename_component(KERNEL_FILEPATH kernels.cl ABSOLUTE CACHE)

target_compile_definitions(${PROJ_NAME} PUBLIC -DKERNEL_FILEPATH="${KERNEL_FILEPATH}")

target_include_directories(${PROJ_NAME} PUBLIC ${OPENCL_INCLUDE_PATH})

# Link Libraries
target_link_libraries(${PROJ_NAME} PUBLIC re_core)
target_link_libraries(${PROJ_NAME} PUBLIC ${OPENCL_LIB})

# Specify the subprojects that depend on OpenCL
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/list_devices")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/precompiler")
