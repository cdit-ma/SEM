
if(MSVC)
    set(TEST_OUTPUT_DIR "")
else()
    set(TEST_OUTPUT_DIR "/test/gtest")
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${TEST_OUTPUT_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_$<CONFIG> ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${TEST_OUTPUT_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/${TEST_OUTPUT_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_$<CONFIG> ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/${TEST_OUTPUT_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}/${TEST_OUTPUT_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_$<CONFIG> ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}/${TEST_OUTPUT_DIR})

if(MSVC)
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
endif(MSVC)

add_executable(test_sem_netdata)

target_sources(test_sem_netdata
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/mock_chart_listener.h
        ${CMAKE_CURRENT_SOURCE_DIR}/chart_json_parser_tests.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/device_sample_aggregator_tests.cpp
        )

target_link_libraries(test_sem_netdata
        PRIVATE
        gtest_main
        gmock
        sem_netdata_adapter
        )

add_executable(sem_netdata_test_client)

target_sources(sem_netdata_test_client
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/test_client.cpp
        )

target_link_libraries(sem_netdata_test_client
        PRIVATE
        sem_netdata_adapter
        )