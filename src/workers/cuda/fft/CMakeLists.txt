cmake_minimum_required(VERSION 3.10)

set(CUDA_WORKER_FFT_LIB "cuda_worker_fft")
add_library(${CUDA_WORKER_FFT_LIB} SHARED "")


set_target_properties(${CUDA_WORKER_FFT_LIB}
        PROPERTIES
        CUDA_SEPARABLE_COMPILATION ON
            POSITION_INDEPENDENT_CODE ON
            CUDA_STANDARD 14)

# Specify the libraries and include directories that the cuda fft code is dependent upon
target_link_libraries(${CUDA_WORKER_FFT_LIB}
        PUBLIC
            cuda_worker_common
            CUDA::cudart
            CUDA::cufft)

target_sources(${CUDA_WORKER_FFT_LIB}
        PUBLIC
            ${CMAKE_CURRENT_SOURCE_DIR}/cuffthandle.cuh
            ${CMAKE_CURRENT_SOURCE_DIR}/fftexecutor.h
            ${CMAKE_CURRENT_SOURCE_DIR}/cufft_error.h
        PRIVATE
            cuffthandle.cu
            cufft_error.cpp)

target_include_directories(${CUDA_WORKER_FFT_LIB}
        INTERFACE
            ${CMAKE_CURRENT_SOURCE_DIR}
        )

if (BUILD_TEST)
    add_subdirectory(test)
endif ()