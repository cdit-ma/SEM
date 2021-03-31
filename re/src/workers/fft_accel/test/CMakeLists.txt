add_executable(test_sem_fft_accel)

add_library(test_sem_fft_accel_udp_loopback
        STATIC
        EXCLUDE_FROM_ALL)

target_sources(test_sem_fft_accel_udp_loopback
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/udp_packet_loopback.hpp
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/udp_packet_loopback.cpp
        )

target_link_libraries(test_sem_fft_accel_udp_loopback
        PUBLIC
        sem_fft_accel_data
        Boost::boost
        Boost::system
        Boost::regex
        Boost::dynamic_linking
        )

target_sources(test_sem_fft_accel
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/worker_test.cpp
        )

target_link_libraries(test_sem_fft_accel
        PUBLIC
        gtest_main
        sem_fft_accel_fpga_worker
        test_sem_fft_accel_udp_loopback
        test_sem_fft_accel_runtime_util
        )
if(MSVC)
    set(TEST_OUTPUT_DIR "")
else()
    set(TEST_OUTPUT_DIR "/test/gtest")
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${TEST_OUTPUT_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_$<CONFIG> ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${TEST_OUTPUT_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/${TEST_OUTPUT_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_$<CONFIG> ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/${TEST_OUTPUT_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}/${TEST_OUTPUT_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_$<CONFIG> ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}/${TEST_OUTPUT_DIR})

if(MSVC)
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
endif(MSVC)

add_subdirectory(fpga_client)