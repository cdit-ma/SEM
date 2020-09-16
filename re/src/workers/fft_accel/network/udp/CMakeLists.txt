# New Boost::Asio changes mean that we should consider bumping this to 1.68
find_package(Boost 1.67.0 REQUIRED COMPONENTS thread regex)

add_library(sem_fft_accel_udp STATIC)

target_sources(sem_fft_accel_udp
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/udp_adapter.hpp

        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/udp_adapter.cpp
        )

target_include_directories(sem_fft_accel_udp
        PUBLIC
        ${PROJECT_SOURCE_DIR}
        )

target_link_libraries(sem_fft_accel_udp
        PUBLIC
        sem_fft_accel_network
        re_types
        )

add_subdirectory(test)