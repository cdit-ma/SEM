
set(PROJ_NAME opencl_worker)
set(TESTER_NAME ${PROJ_NAME}_tester)
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

# Specify the subprojects that depend on OpenCL
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/list_devices")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/precompiler")

# Apply overrides to OpenCL include directory if specified
set(OPENCL_INCLUDE_PATH $ENV{OPENCL_INCLUDE_PATH})
if (NOT OPENCL_INCLUDE_PATH)
    message(STATUS "Setting opencl include directory to default (${OPENCL_ROOT}/include)")
    set(OPENCL_INCLUDE_PATH "${OPENCL_ROOT}/include")
endif()

set(SOURCE
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
    )

set(HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/cl2.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/openclutilities.h
    ${CMAKE_CURRENT_SOURCE_DIR}/openclmanager.h
    ${CMAKE_CURRENT_SOURCE_DIR}/genericbuffer.h
    ${CMAKE_CURRENT_SOURCE_DIR}/oclbuffer.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/openclbufferbase.h
    ${CMAKE_CURRENT_SOURCE_DIR}/openclkernelbase.h
    ${CMAKE_CURRENT_SOURCE_DIR}/openclkernel.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/opencl_worker.h
    ${CMAKE_CURRENT_SOURCE_DIR}/openclloadbalancer.h
    ${CMAKE_CURRENT_SOURCE_DIR}/opencldevice.h
    ${CMAKE_CURRENT_SOURCE_DIR}/openclexception.h
    ${CMAKE_CURRENT_SOURCE_DIR}/opencldevice.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/fft/openclfft_worker.h
    )

# Search for OpenCL FFT libraries
set(CLFFT_ROOT $ENV{CLFFT_ROOT})
set(CLFFT_LIB_ROOT $ENV{CLFFT_LIB_ROOT})
find_library(CLFFT_LIB clFFT ${CLFFT_LIB_ROOT})

# Add implementations for fft library as necessary
if(ALTERA_OCLSDK_LIB)
    message(STATUS "building OpenCL worker with FPGA FFT library")
    # set(FFT_LIB FPGA)
    set(SOURCE
        ${SOURCE}
        ${CMAKE_CURRENT_SOURCE_DIR}/fft1d/opencl_worker_alterafpga.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/fft1d/opencl.cpp
        )
elseif (CLFFT_LIB)
    message(STATUS "building OpenCL worker with GPU FFT library (clFFT)")
    set(SOURCE
        ${SOURCE}
        ${CMAKE_CURRENT_SOURCE_DIR}/opencl_worker_clfft.cpp
        )
else()
    message(STATUS "No FFT library found when building OpenCL worker")
    set(SOURCE
    ${SOURCE}
    ${CMAKE_CURRENT_SOURCE_DIR}/opencl_worker_fftdummy.cpp
    )
endif()


# Mark library dependencies
add_library(${PROJ_NAME} SHARED ${SOURCE} ${HEADERS})
add_executable(${TESTER_NAME} openclmanagertests.cpp)

# Check for whether FFT implementation should use GPU or FPGA (preference FPGA if avaialable)
if(ALTERA_OCLSDK_LIB)
    message(STATUS "Building OpenCL worker with FPGA FFT library")
    target_compile_definitions(${PROJ_NAME} PRIVATE -DFFT_LIBRARY='2')
    #target_include_directories(${PROJ_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/fft1d")
elseif(CLFFT_LIB)
    target_compile_definitions(${PROJ_NAME} PRIVATE -DFFT_LIBRARY='1')
    target_include_directories(${PROJ_NAME} PRIVATE "${CLFFT_ROOT}/include")
    target_link_libraries(${PROJ_NAME} ${CLFFT_LIB})
    target_link_libraries(${TESTER_NAME} ${CLFFT_LIB})
else()
    message(STATUS "No FFT library found when building OpenCL worker")
endif()


# Specify the location of kernels - consider moving kernels to be exported alongside binaries?
get_filename_component(KERNEL_FILEPATH kernels.cl ABSOLUTE CACHE)
message(STATUS "KERNAL FILEPATH: ${KERNEL_FILEPATH}")
target_compile_definitions(${PROJ_NAME} PUBLIC -DKERNEL_FILEPATH="${KERNEL_FILEPATH}")

#Add RE_PATH to include directories
target_include_directories(${PROJ_NAME} PRIVATE ${RE_SRC_PATH})
target_include_directories(${PROJ_NAME} PRIVATE ${OPENCL_INCLUDE_PATH})
#target_include_directories(${PROJ_NAME} PRIVATE "${CLFFT_ROOT}/include")

target_include_directories(${TESTER_NAME} PRIVATE ${RE_SRC_PATH})
target_include_directories(${TESTER_NAME} PRIVATE ${OPENCL_INCLUDE_PATH})
#target_include_directories(${TESTER_NAME} PRIVATE "${CLFFT_ROOT}/include")

if (MSVC)
 	#Visual studio needs to be told to build in Multithreaded Dynamically Linked mode
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MD")
elseif(UNIX)
	target_link_libraries(${PROJ_NAME} pthread)
endif(MSVC)


target_link_libraries(${PROJ_NAME} re_core)
target_link_libraries(${PROJ_NAME} ${OPENCL_LIB})
#target_link_libraries(${PROJ_NAME} ${CLFFT_LIB})

target_link_libraries(${TESTER_NAME} re_core)
target_link_libraries(${TESTER_NAME} ${PROJ_NAME})
target_link_libraries(${TESTER_NAME} ${OPENCL_LIB})
#target_link_libraries(${TESTER_NAME} ${CLFFT_LIB})
