cmake_minimum_required (VERSION 2.6)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

SET(CMAKE_CXX_FLAGS "-O0")
SET(CMAKE_C_FLAGS "-O0")

set(PROFILER_NAME cpu_profiler)

project(profiler)

set(SOURCE
    ${CMAKE_CURRENT_SOURCE_DIR}/../cpu_worker_impl.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../cpu_worker_impl.h
    ${CMAKE_CURRENT_SOURCE_DIR}/../intop.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../floatop.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../dhry_1.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../whets.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../dhry.h
    ${CMAKE_CURRENT_SOURCE_DIR}/cpu_worker_profiler.cpp
    )

add_executable(${PROFILER_NAME} ${SOURCE})

