
find_package(Boost 1.67.0 REQUIRED COMPONENTS program_options)

add_executable(fpga_fft_client)

target_sources(fpga_fft_client
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/fft_client.cpp
        )

target_link_libraries(fpga_fft_client
        PRIVATE
        sem_fft_accel_fpga_worker
        Boost::program_options
)