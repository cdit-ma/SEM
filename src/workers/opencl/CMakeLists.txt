set(PROJ_NAME opencl_worker)
project(${PROJ_NAME}
        VERSION ${PROJECT_VERSION})

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
target_compile_features(${PROJ_NAME} PRIVATE cxx_std_17)
# Suppress ignored cl_int alignment attribute warning in opencl library
target_compile_options(${PROJ_NAME} PRIVATE -Wno-ignored-attributes)

target_sources(${PROJ_NAME} PRIVATE
    # Sources
    ${CMAKE_CURRENT_SOURCE_DIR}/utilities.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/manager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/genericbuffer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/bufferbase.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/kernel.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/kernelbase.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/opencl_worker.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/loadbalancer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/device.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/openclexception.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/queue.cpp
    #${CMAKE_CURRENT_SOURCE_DIR}/fpgafftprovider.cpp


    # Headers
    ${CMAKE_CURRENT_SOURCE_DIR}/cl2.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/utilities.h
    ${CMAKE_CURRENT_SOURCE_DIR}/manager.h
    ${CMAKE_CURRENT_SOURCE_DIR}/genericbuffer.h
    ${CMAKE_CURRENT_SOURCE_DIR}/buffer.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/bufferbase.h
    ${CMAKE_CURRENT_SOURCE_DIR}/kernelbase.h
    ${CMAKE_CURRENT_SOURCE_DIR}/kernel.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/opencl_worker.h
    ${CMAKE_CURRENT_SOURCE_DIR}/loadbalancer.h
    ${CMAKE_CURRENT_SOURCE_DIR}/device.h
    ${CMAKE_CURRENT_SOURCE_DIR}/openclexception.h
    ${CMAKE_CURRENT_SOURCE_DIR}/queue.h
    ${CMAKE_CURRENT_SOURCE_DIR}/fftprovider.h
    ${CMAKE_CURRENT_SOURCE_DIR}/clfftprovider.h
    ${CMAKE_CURRENT_SOURCE_DIR}/fpgafftprovider.h
)

# Search for OpenCL FFT libraries
set(CLFFT_ROOT $ENV{CLFFT_ROOT})
set(CLFFT_LIB_ROOT $ENV{CLFFT_LIB_ROOT})

find_library(CLFFT_LIB clFFT ${CLFFT_LIB_ROOT})

# Add implementations for fft library as necessary
# TODO: Break these out into the appropriate sub-projects in order to keep this file manageable
if(ALTERA_OCLSDK_LIB)
    message(STATUS "Building OpenCL worker with FPGA FFT library")
    target_sources(${PROJ_NAME} PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/fpgafftprovider.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/fft1d/opencl.cpp
    )
    target_compile_definitions(${PROJ_NAME} PUBLIC FPGA_PRESENT)
elseif (CLFFT_LIB)
    message(STATUS "Building OpenCL worker with GPU FFT library (clFFT)")
    target_sources(${PROJ_NAME} PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/clfftprovider.cpp
    )
    target_compile_definitions(${PROJ_NAME} PUBLIC CLFFT_PRESENT)
else()
    message(STATUS "No FFT library found when building OpenCL worker")
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
