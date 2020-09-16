add_library(sem_fft_accel_udp_adapter_tests STATIC)

target_sources(sem_fft_accel_udp_adapter_tests
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/udp_adapter_tests.cpp
        )

target_link_libraries(sem_fft_accel_udp_adapter_tests
        PUBLIC
        gtest
        sem_fft_accel_udp
)