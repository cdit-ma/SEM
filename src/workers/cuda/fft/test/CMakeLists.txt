cmake_minimum_required(VERSION 3.10)

add_executable(cuda_worker_fft_test)

# Specify the compiler settings for this library
target_compile_features(cuda_worker_fft_test
        PRIVATE
            cxx_std_17)

set_target_properties(cuda_worker_fft_test
        PROPERTIES
            CUDA_SEPARABLE_COMPILATION ON
            POSITION_INDEPENDENT_CODE ON)

target_link_libraries(cuda_worker_fft_test
        PUBLIC
            cuda_worker_fft
            gtest
            gtest_main)

target_sources(cuda_worker_fft_test
        PRIVATE
            test_fft.cpp)