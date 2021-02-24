add_library(sem_fft_accel_data
        STATIC
        EXCLUDE_FROM_ALL
        )

target_sources(sem_fft_accel_data
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/fft_data.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/serialized.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/byte_span.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/packet_headers.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/fft_packet_group.hpp
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/fft_data.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/packet_headers.cpp
        )

target_include_directories(sem_fft_accel_data
        PUBLIC
        ${PROJECT_SOURCE_DIR}
        )

target_link_libraries(sem_fft_accel_data
        PRIVATE
        Boost::boost
        )

if(BUILD_TEST)
    add_subdirectory(test)
endif()