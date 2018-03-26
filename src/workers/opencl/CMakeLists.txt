add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/list_devices")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/precompiler")

set(PROJ_NAME opencl_worker)
set(TESTER_NAME ${PROJ_NAME}_tester)

set(OPENCL_ROOT $ENV{OPENCL_ROOT})
set(OPENCL_LIB_PATH $ENV{OPENCL_LIB_PATH})
#set(CLFFT_ROOT $ENV{CLFFT_ROOT})
#set(CLFFT_LIB_ROOT $ENV{CLFFT_LIB_ROOT})
project(${PROJ_NAME})

find_library(OPENCL_LIB OpenCL ${OPENCL_LIB_PATH})
#find_library(CLFFT_LIB clFFT ${CLFFT_LIB_ROOT})


if(NOT OPENCL_LIB)
    message(STATUS "Not Found OpenCL - Not building opencl_worker")
    return()
endif()

#if(NOT CLFFT_LIB)
#    message(STATUS "Not Found CLFFT - Not building GPU worker")
#    return()
#endif(NOT CLFFT_LIB)
#add_definitions(-DKERNEL_FILEPATH="${KERNEL_FILEPATH}")

set(SOURCE
    ${CMAKE_CURRENT_SOURCE_DIR}/openclutilities.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/openclmanager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/genericbuffer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/openclkernel.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/openclkernelbase.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/opencl_worker.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/openclloadbalancer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/opencldevice.cpp
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
    )

#Use this for enable_testing
add_library(${PROJ_NAME} SHARED ${SOURCE} ${HEADERS})
add_executable(${TESTER_NAME} openclmanagertests.cpp)

get_filename_component(KERNEL_FILEPATH kernels.cl ABSOLUTE CACHE)
message(${KERNEL_FILEPATH})
target_compile_definitions(${PROJ_NAME} PRIVATE -DKERNEL_FILEPATH="${KERNEL_FILEPATH}")

#Add RE_PATH to include directories
target_include_directories(${PROJ_NAME} PRIVATE ${RE_SRC_PATH})
target_include_directories(${PROJ_NAME} PRIVATE "${OPENCL_ROOT}/include")
#target_include_directories(${PROJ_NAME} PRIVATE "${CLFFT_ROOT}/include")

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
