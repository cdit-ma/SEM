# Generic FFT Acceleration Worker library
add_library(sem_fft_accel_runtime
        STATIC
        EXCLUDE_FROM_ALL
        )

set_target_properties(sem_fft_accel_runtime PROPERTIES POSITION_INDEPENDENT_CODE ON)
target_compile_features(sem_fft_accel_runtime
        PUBLIC
        cxx_std_17
        )

target_sources(sem_fft_accel_runtime
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/adapter.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/adapter_impl.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/fft_request_map.hpp

        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/adapter.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/adapter_impl.cpp
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
        sem_fft_accel_data
        )

if (BUILD_TEST)
    add_subdirectory(test)
endif()