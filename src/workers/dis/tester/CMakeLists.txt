set(PROJ_NAME dis_worker_tester)
project(${PROJ_NAME})

set(SOURCE
    ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
    )

set(HEADERS
    )

add_executable(${PROJ_NAME} ${SOURCE} ${HEADERS})

target_link_libraries(${PROJ_NAME} PUBLIC dis_worker)

