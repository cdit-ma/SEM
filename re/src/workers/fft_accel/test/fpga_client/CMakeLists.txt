
find_package(Boost 1.67.0 REQUIRED COMPONENTS program_options filesystem)

add_executable(fpga_fft_client)

target_compile_features(fpga_fft_client
        PUBLIC
        cxx_std_17
        )

target_sources(fpga_fft_client
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/fft_client.cpp
        )

target_link_libraries(fpga_fft_client
        PRIVATE
        sem_fft_accel_fpga_worker
        Boost::program_options
        Boost::filesystem
)