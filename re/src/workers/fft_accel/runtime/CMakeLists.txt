# Generic FFT Acceleration Worker library
add_library(sem_fft_accel_runtime STATIC)

target_compile_features(sem_fft_accel_runtime
        PUBLIC
        cxx_std_17
        )

target_sources(sem_fft_accel_runtime
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/adapter.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/worker_adapter.hpp

        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/adapter.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/worker_adapter.cpp
        )

target_include_directories(sem_fft_accel_runtime
        INTERFACE
        ${PROJECT_SOURCE_DIR}
        )

target_link_libraries(sem_fft_accel_runtime
        PUBLIC
        re_core
        re_types
        re_loggers
        sem_fft_accel_network
        )

add_subdirectory(test)