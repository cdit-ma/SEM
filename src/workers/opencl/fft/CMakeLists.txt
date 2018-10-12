
set(PROJ_NAME opencl_fft_worker)

set(OPENCL_ROOT $ENV{OPENCL_ROOT})
set(OPENCL_LIB_PATH $ENV{OPENCL_LIB_PATH})
project(${PROJ_NAME})

find_library(OPENCL_LIB OpenCL ${OPENCL_LIB_PATH})

# If OpenCL doesn't exist on this machine we can bail out
if(NOT OPENCL_LIB)
    message(STATUS "Not Found OpenCL - Not building opencl_fft_worker")
    return()
endif()

# Search for OpenCL FFT libraries
set(CLFFT_ROOT $ENV{CLFFT_ROOT})
set(CLFFT_LIB_ROOT $ENV{CLFFT_LIB_ROOT})
find_library(CLFFT_LIB clFFT ${CLFFT_LIB_ROOT})

# Check for whether FFT implementation should use GPU or FPGA (preference FPGA if avaialable)
if(FPGA_FFT_LIB)
    message(STATUS "building OpenCL worker with FPGA FFT library")
    set(FFT_LIB FPGA)
    set(SOURCE
        ${CMAKE_CURRENT_SOURCE_DIR}/openclfftfpgawrapper.cpp
        )
elseif (CLFFT_LIB)
    message(STATUS "building OpenCL worker with GPU FFT library")
    set(FFT_LIB GPU)
    set(SOURCE
        ${CMAKE_CURRENT_SOURCE_DIR}/openclfftgpuwrapper.cpp
        )
else()
    message(STATUS "No FFT library found when building OpenCL worker")
endif(FPGA_FFT_LIB)

set(SOURCE
    ${CMAKE_CURRENT_SOURCE_DIR}/openclfft_worker.cpp
    )

set(HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/openclfft_worker.h
    )

# Mark library dependencies
add_library(${PROJ_NAME} STATIC ${SOURCE} ${HEADERS})

#Add RE_PATH to include directories
target_include_directories(${PROJ_NAME} PUBLIC ${RE_SOURCE_PATH})
target_include_directories(${PROJ_NAME} PUBLIC "${OPENCL_ROOT}/include")
target_include_directories(${PROJ_NAME} PUBLIC "${CLFFT_ROOT}/include")


if (MSVC)
 	#Visual studio needs to be told to build in Multithreaded Dynamically Linked mode
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MD")
elseif(UNIX)
	target_link_libraries(${PROJ_NAME} PUBLIC pthread)
endif(MSVC)


target_link_libraries(${PROJ_NAME} PUBLIC re_core)
target_link_libraries(${PROJ_NAME} PUBLIC opencl_worker)
target_link_libraries(${PROJ_NAME} PUBLIC ${OPENCL_LIB})
target_link_libraries(${PROJ_NAME} PUBLIC ${CLFFT_LIB})