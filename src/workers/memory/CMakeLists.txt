set(PROJ_NAME memory_worker)
project(${PROJ_NAME})

add_library(${PROJ_NAME} SHARED)

target_sources(${PROJ_NAME} PRIVATE
    # Sources
    ${CMAKE_CURRENT_SOURCE_DIR}/memory_worker.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/memory_worker_impl.cpp
    # Headers
    ${CMAKE_CURRENT_SOURCE_DIR}/memory_worker.h
    ${CMAKE_CURRENT_SOURCE_DIR}/memory_worker_impl.h
)

# Link Libraries
target_link_libraries(${PROJ_NAME} PUBLIC re_core)
