add_library(test_sem_fft_accel_data_util
        INTERFACE
        )

target_sources(test_sem_fft_accel_data_util
        INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}/data_test_util.hpp
        )

target_include_directories(test_sem_fft_accel_data_util
        INTERFACE
        ${CMAKE_PROJECT_SOURCE_DIR}
        )

target_sources(test_sem_fft_accel
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/fft_data_tests.cpp
        )

target_link_libraries(test_sem_fft_accel
        PUBLIC
        gtest
        gmock
        sem_fft_accel_data
        test_sem_fft_accel_data_util
        )