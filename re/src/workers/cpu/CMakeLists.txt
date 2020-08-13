add_library(cpu_worker SHARED)
target_compile_features(cpu_worker PRIVATE cxx_std_11)
set_target_properties(cpu_worker PROPERTIES POSITION_INDEPENDENT_CODE ON)
# Disable Optimizing
if (UNIX)
    target_compile_options(cpu_worker PRIVATE "-O0")
elseif(MSVC AND CMAKE_BUILD_TYPE STREQUAL "Release")
    #target_compile_options(cpu_worker PRIVATE "/Od /wd4996 /wd4244")
    message(WARNING "CPU_WORKER compiled with optimisations. This will change results.")
endif(UNIX)

target_sources(cpu_worker PRIVATE
    # Sources
    ${CMAKE_CURRENT_SOURCE_DIR}/cpu_worker.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/cpu_worker_impl.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/intop.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/floatop.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/whets.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/dhry_1.cpp
    # Headers
    ${CMAKE_CURRENT_SOURCE_DIR}/cpu_worker_impl.h
    ${CMAKE_CURRENT_SOURCE_DIR}/cpu_worker.h
    ${CMAKE_CURRENT_SOURCE_DIR}/dhry.h
)
target_link_libraries(cpu_worker
        PUBLIC 
        re_core
        )

target_include_directories(cpu_worker PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/profiler")
