add_library(sem_fft_accel_data
        STATIC
        EXCLUDE_FROM_ALL
        )

target_sources(sem_fft_accel_data
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/fft_data.hpp
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/fft_data.cpp
        )

target_include_directories(sem_fft_accel_data
        PUBLIC
        ${PROJECT_SOURCE_DIR}
        )

if(BUILD_TEST)
    add_subdirectory(test)
endif()