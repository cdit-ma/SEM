
add_library(sem_fft_accel_network
        STATIC
        EXCLUDE_FROM_ALL
        )

set_target_properties(sem_fft_accel_network PROPERTIES POSITION_INDEPENDENT_CODE ON)

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
        sem_types
)

add_subdirectory(udp)