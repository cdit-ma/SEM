
add_library(sem_fft_accel_network STATIC)

target_sources(sem_fft_accel_network
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/adapter.hpp

        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/adapter.cpp
        )

target_include_directories(sem_fft_accel_network
        PUBLIC
        ${PROJECT_SOURCE_DIR}
        )

target_link_libraries(sem_fft_accel_network
        PUBLIC
        re_types
)

add_subdirectory(udp)