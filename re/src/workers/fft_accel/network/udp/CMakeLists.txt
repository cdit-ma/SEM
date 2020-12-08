# New Boost::Asio changes mean that we should consider bumping this to 1.68
find_package(Boost 1.67.0 REQUIRED COMPONENTS system)

add_library(sem_fft_accel_udp
        STATIC
        EXCLUDE_FROM_ALL
        )

set_target_properties(sem_fft_accel_udp PROPERTIES POSITION_INDEPENDENT_CODE ON)

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
        Boost::boost
        Boost::system
        Boost::dynamic_linking
        )

if (BUILD_TEST)
    add_subdirectory(test)
endif()