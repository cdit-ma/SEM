set(PROJ_NAME cpu_profiler)
project(${PROJ_NAME})

set(SOURCE
    ${CMAKE_CURRENT_SOURCE_DIR}/cpu_worker_profiler.cpp
    )

add_executable(${PROJ_NAME} ${SOURCE} ${HEADERS})

# Link Libraries
target_link_libraries(${PROJ_NAME} PRIVATE cpu_worker)