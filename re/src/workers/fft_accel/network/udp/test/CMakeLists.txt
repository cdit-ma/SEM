add_library(sem_fft_accel_udp_adapter_tests SHARED)

target_sources(sem_fft_accel_udp_adapter_tests
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/udp_adapter_tests.cpp
        #${CMAKE_CURRENT_SOURCE_DIR}/mock_fft_result_listener.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/async_fft_result_listener.hpp
        )

target_link_libraries(sem_fft_accel_udp_adapter_tests
        PUBLIC
        gtest
        gmock
        sem_fft_accel_udp
        re_types
)