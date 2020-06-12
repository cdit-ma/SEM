cmake_minimum_required(VERSION 3.10)

add_library(cuda_worker_common SHARED "")

set_target_properties(cuda_worker_common
        PROPERTIES
            CUDA_SEPARABLE_COMPILATION ON
            POSITION_INDEPENDENT_CODE ON
            CUDA_STANDARD 14)

# Specify the libraries and include directories that the cuda fft code is dependent upon
target_link_libraries(cuda_worker_common
        PUBLIC
            CUDA::cudart)


target_sources(cuda_worker_common
        PUBLIC
            ${CMAKE_CURRENT_SOURCE_DIR}/devicelist.cuh
        PRIVATE
            devicelist.cu)


target_include_directories(cuda_worker_common
        INTERFACE
            ${CMAKE_CURRENT_SOURCE_DIR}
        )