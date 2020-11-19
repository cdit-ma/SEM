project(sem_fft_acceleration)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Worker library to be exposed to RE
add_library(sem_fft_accel_fpga_worker SHARED)

# Single executable for all tests within the project
if (BUILD_TEST)
    add_subdirectory(test)
endif()

add_subdirectory(data)
add_subdirectory(network)
add_subdirectory(runtime)

target_compile_features(sem_fft_accel_fpga_worker
        INTERFACE
        cxx_std_17
        )
target_sources(sem_fft_accel_fpga_worker
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/sem_fft_accel_worker.hpp
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/sem_fft_accel_worker.cpp
        )

target_include_directories(sem_fft_accel_fpga_worker
        INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}
        )

target_link_libraries(sem_fft_accel_fpga_worker
        PUBLIC
        sem_fft_accel_runtime
        re_core
        sem_fft_accel_udp
        )

#export(TARGETS
#        sem_fft_accel_fpga_worker
#        NAMESPACE sem::
#        APPEND
#        FILE ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/sem_exports.cmake
#        EXPORT_LINK_INTERFACE_LIBRARIES
#        )

#if (BUILD_TEST)

#endif()