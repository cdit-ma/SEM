project(sem_fft_acceleration)

add_subdirectory(data)
add_subdirectory(network)
add_subdirectory(runtime)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Worker library to be exposed to RE
add_library(sem_fft_accel_fpga_worker SHARED)

target_compile_features(sem_fft_accel_fpga_worker
        PRIVATE
        cxx_std_17
        )

target_sources(sem_fft_accel_fpga_worker
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/sem_fft_accel_worker.hpp
        )

target_include_directories(sem_fft_accel_fpga_worker
        INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}
        )

target_link_libraries(sem_fft_accel_fpga_worker
        PUBLIC
        sem_fft_accel_runtime
        sem_fft_accel_udp
        )

add_executable(sem_fft_accel_test)

target_link_libraries(sem_fft_accel_test
        PUBLIC
        gtest_main
        sem_fft_accel_runtime_test
        sem_fft_accel_udp_adapter_tests
        )