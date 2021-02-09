#add_library(sem_fft_accel_udp_adapter_tests SHARED)

target_sources(test_sem_fft_accel
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/udp_adapter_tests.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/async_fft_result_listener.hpp
        )

target_link_libraries(test_sem_fft_accel
        PUBLIC
        gtest
        gmock
        sem_fft_accel_udp
        re_types
        test_sem_fft_accel_data_util
)