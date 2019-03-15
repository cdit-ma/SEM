set(PROJ_NAME cpu_profiler)
project(${PROJ_NAME})

add_executable(${PROJ_NAME})

target_sources(${PROJ_NAME} PRIVATE
    # Sources
    ${CMAKE_CURRENT_SOURCE_DIR}/cpu_worker_profiler.cpp
)

# Link Libraries
target_link_libraries(${PROJ_NAME} PRIVATE cpu_worker)