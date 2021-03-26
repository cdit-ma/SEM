add_executable(sem_netdata_test_client)

target_sources(sem_netdata_test_client
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/test_client.cpp
        )

target_link_libraries(sem_netdata_test_client
        PRIVATE
        sem_netdata_adapter
        )