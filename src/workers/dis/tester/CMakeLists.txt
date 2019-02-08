set(PROJ_NAME dis_worker_tester)
set(TX_NAME dis_tx)
set(RX_NAME dis_rx)
project(${PROJ_NAME})

set(SOURCE
    ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
    )

set(HEADERS
    )

add_executable(${PROJ_NAME} ${SOURCE} ${HEADERS})

add_executable(${TX_NAME} tx.cpp)
add_executable(${RX_NAME} rx.cpp)

target_link_libraries(${PROJ_NAME} PUBLIC dis_worker)
target_link_libraries(${TX_NAME} PUBLIC dis_worker)
target_link_libraries(${RX_NAME} PUBLIC dis_worker)

