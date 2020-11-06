project(sem_fft_acceleration)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Worker library to be exposed to RE
add_library(sem_fft_accel_fpga_worker SHARED)

# Single executable for all tests within the project
if (BUILD_TEST)
    add_executable(test_sem_fft_accel)
endif()

add_subdirectory(data)
add_subdirectory(network)
add_subdirectory(runtime)

target_compile_features(sem_fft_accel_fpga_worker
        PRIVATE
        cxx_std_17
        )
target_sources(sem_fft_accel_fpga_worker
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/sem_fft_accel_worker.hpp
        )

target_include_directories(sem_fft_accel_fpga_worker
        INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}
        )

target_link_libraries(sem_fft_accel_fpga_worker
        PUBLIC
        sem_fft_accel_runtime
        sem_fft_accel_udp
        )

#export(TARGETS
#        sem_fft_accel_fpga_worker
#        NAMESPACE sem::
#        APPEND
#        FILE ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/sem_exports.cmake
#        EXPORT_LINK_INTERFACE_LIBRARIES
#        )

if (BUILD_TEST)
    target_link_libraries(test_sem_fft_accel
            PUBLIC
            gtest_main
            #sem_fft_accel_runtime_test
            #sem_fft_accel_udp_adapter_tests
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
endif()