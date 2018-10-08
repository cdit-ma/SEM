set(PROJ_NAME memory_worker)
project(${PROJ_NAME})

set(SOURCE
    ${CMAKE_CURRENT_SOURCE_DIR}/memory_worker.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/memory_worker_impl.cpp
    )

set(HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/memory_worker.h
    ${CMAKE_CURRENT_SOURCE_DIR}/memory_worker_impl.h
    )

add_library(${PROJ_NAME} STATIC ${SOURCE} ${HEADERS})

# Link Libraries
target_link_libraries(${PROJ_NAME} re_core)
