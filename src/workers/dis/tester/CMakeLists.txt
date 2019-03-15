set(PROJ_NAME dis_worker_tester)
set(TX_NAME dis_tx)
set(RX_NAME dis_rx)
project(${PROJ_NAME})

add_executable(${PROJ_NAME})
add_executable(${TX_NAME})
add_executable(${RX_NAME})

target_sources(${PROJ_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp)
target_sources(${TX_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/tx.cpp)
target_sources(${RX_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/rx.cpp)

target_link_libraries(${PROJ_NAME} PUBLIC dis_worker)
target_link_libraries(${TX_NAME} PUBLIC dis_worker)
target_link_libraries(${RX_NAME} PUBLIC dis_worker)