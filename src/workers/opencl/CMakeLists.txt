
set(PROJ_NAME opencl_worker)
set(TESTER_NAME ${PROJ_NAME}_tester)

set(OPENCL_ROOT $ENV{OPENCL_ROOT})
set(OPENCL_LIB_PATH $ENV{OPENCL_LIB_PATH})
project(${PROJ_NAME})

find_library(OPENCL_LIB OpenCL ${OPENCL_LIB_PATH})

# If OpenCL doesn't exist on this machine we can bail out
if(NOT OPENCL_LIB)
    message(STATUS "Not Found OpenCL - Not building opencl_worker")
    return()
endif()

# Specify the subprojects that depend on OpenCL
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/list_devices")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/precompiler")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/fft")


# Search for OpenCL FFT libraries
# set(CLFFT_ROOT $ENV{CLFFT_ROOT})
# set(CLFFT_LIB_ROOT $ENV{CLFFT_LIB_ROOT})
# find_library(CLFFT_LIB clFFT ${CLFFT_LIB_ROOT})

# Check for whether FFT implementation should use GPU or FPGA (preference FPGA if avaialable)
# if(FPGA_FFT_LIB)
#     message(STATUS "building OpenCL worker with FPGA FFT library")
#     set(FFT_LIB FPGA)
#     set(FFT_SOURCE
#         ${CMAKE_CURRENT_SOURCE_DIR}/openclfftfpgawrapper.cpp
#         )
# elseif (CLFFT_LIB)
#     message(STATUS "building OpenCL worker with GPU FFT library")
#     set(FFT_LIB GPU)
#     set(FFT_SOURCE
#         ${CMAKE_CURRENT_SOURCE_DIR}/openclfftgpuwrapper.cpp
#         )
# else()
#     message(STATUS "No FFT library found when building OpenCL worker")
# endif(FPGA_FFT_LIB)


set(SOURCE
    ${CMAKE_CURRENT_SOURCE_DIR}/openclutilities.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/openclmanager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/genericbuffer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/openclkernel.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/openclkernelbase.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/opencl_worker.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/openclloadbalancer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/opencldevice.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/openclexception.cpp
    )

set(HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/cl2.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/openclutilities.h
    ${CMAKE_CURRENT_SOURCE_DIR}/openclmanager.h
    ${CMAKE_CURRENT_SOURCE_DIR}/genericbuffer.h
    ${CMAKE_CURRENT_SOURCE_DIR}/oclbuffer.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/openclkernelbase.h
    ${CMAKE_CURRENT_SOURCE_DIR}/openclkernel.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/opencl_worker.h
    ${CMAKE_CURRENT_SOURCE_DIR}/openclloadbalancer.h
    ${CMAKE_CURRENT_SOURCE_DIR}/opencldevice.h
    ${CMAKE_CURRENT_SOURCE_DIR}/openclexception.h
    ${CMAKE_CURRENT_SOURCE_DIR}/fft/openclfft_worker.h
    )

# Mark library dependencies
add_library(${PROJ_NAME} SHARED ${SOURCE} ${HEADERS})
# if(FFT_LIB)
#     add_library(${PROJ_NAME}_FFT SHARED ${FFT_SOURCE} ${CMAKE_CURRENT_SOURCE_DIR}/openclfft.h)
#     target_include_directories(${PROJ_NAME} PRIVATE "${CLFFT_ROOT}/include")
# endif()
add_executable(${TESTER_NAME} openclmanagertests.cpp)

# Specify the location of kernels - consider moving kernels to be exported alongside binaries?
get_filename_component(KERNEL_FILEPATH kernels.cl ABSOLUTE CACHE)
target_compile_definitions(${PROJ_NAME} PRIVATE -DKERNEL_FILEPATH="${KERNEL_FILEPATH}")

#Add RE_PATH to include directories
target_include_directories(${PROJ_NAME} PRIVATE ${RE_SRC_PATH})
target_include_directories(${PROJ_NAME} PRIVATE "${OPENCL_ROOT}/include")

target_include_directories(${TESTER_NAME} PRIVATE ${RE_SRC_PATH})
target_include_directories(${TESTER_NAME} PRIVATE "${OPENCL_ROOT}/include")
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
