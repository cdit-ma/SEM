
add_library(test_sem_fft_accel_runtime_util INTERFACE)

target_sources(test_sem_fft_accel_runtime_util
        INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}/callback_handler.hpp)

target_include_directories(test_sem_fft_accel_runtime_util
        INTERFACE
        ${PROJECT_SOURCE_DIR}
        )

target_sources(test_sem_fft_accel
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/event_dispatcher_tests.hpp
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/runtime_adapter_tests.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/single_threaded_dispatcher_tests.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/mock_network_adapter.hpp
        )

target_link_libraries(test_sem_fft_accel
        PUBLIC
        gtest
        gmock
        sem_fft_accel_runtime
        test_sem_fft_accel_runtime_util
        )