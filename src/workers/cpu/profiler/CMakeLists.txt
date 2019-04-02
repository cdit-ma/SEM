set(PROJ_NAME cpu_profiler)
project(${PROJ_NAME})

add_executable(${PROJ_NAME} "")
target_compile_features(${PROJ_NAME} PRIVATE cxx_std_11)

target_sources(${PROJ_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/cpu_worker_profiler.cpp
)
target_link_libraries(${PROJ_NAME} PRIVATE cpu_worker)
