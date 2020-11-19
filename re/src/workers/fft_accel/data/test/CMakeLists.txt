target_sources(test_sem_fft_accel
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/fft_data_tests.cpp
        )

target_link_libraries(test_sem_fft_accel
        PUBLIC
        gtest
        gmock
        sem_fft_accel_data
        )