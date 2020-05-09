cmake_minimum_required(VERSION 3.10)

project(cuda_worker
        VERSION
            ${PROJECT_VERSION}
        LANGUAGES
            C CXX CUDA)


find_package(CUDAToolkit)
if (NOT CUDAToolkit_FOUND)
    message(STATUS
            "CUDA Toolkit not found - not building ${PROJECT_NAME}")
    return()
endif ()

# Libraries
add_subdirectory(common)
add_subdirectory(fft)

# Executables
add_subdirectory(list_devices)

if (BUILD_TEST)
    add_subdirectory(test)
endif ()

# Create a shared library with the name driven by the project name
set(CUDA_WORKER_LIB ${PROJECT_NAME})
add_library(${CUDA_WORKER_LIB} SHARED "")

# Specify the compiler settings for this library
target_compile_features(${CUDA_WORKER_LIB}
        PRIVATE
            cxx_std_17)
set_target_properties(${CUDA_WORKER_LIB}
        PROPERTIES
            CUDA_SEPERABLE_COMPILATION ON
            POSITION_INDEPENDENT_CODE ON)

# Specify the libraries and include directories that the cuda worker is dependent upon
target_link_libraries(${CUDA_WORKER_LIB}
        PUBLIC
            CUDA::cudart
            cuda_worker_fft
            re_core)


target_sources(${CUDA_WORKER_LIB}
        PUBLIC
            ${CMAKE_CURRENT_SOURCE_DIR}/cudaworker.h
        PRIVATE
            cudaworker.cpp)

target_include_directories(${CUDA_WORKER_LIB}
        INTERFACE
            ${CMAKE_CURRENT_SOURCE_DIR}
            ${CMAKE_SOURCE_DIR}/core)
