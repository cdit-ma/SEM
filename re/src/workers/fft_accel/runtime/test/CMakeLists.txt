add_library(sem_fft_accel_runtime_test SHARED)

set_target_properties(sem_fft_accel_runtime_test PROPERTIES POSITION_INDEPENDENT_CODE ON)
target_sources(sem_fft_accel_runtime_test
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/worker_runtime_adapter_tests.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/mock_network_adapter.hpp
        )

target_link_libraries(sem_fft_accel_runtime_test
        PUBLIC
        gtest
        gmock
        sem_fft_accel_runtime
        )