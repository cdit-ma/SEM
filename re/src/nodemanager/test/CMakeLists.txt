
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/test)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}/test)

set(CONFIG_TEST_NAME node_manager_test_config)

add_executable(${CONFIG_TEST_NAME} "")
target_compile_features(${CONFIG_TEST_NAME} PRIVATE cxx_std_17)

target_sources(${CONFIG_TEST_NAME} PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/nodemanagerconfig.cpp
        )
target_include_directories(${CONFIG_TEST_NAME}
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../..
        ${GOOGLE_TEST_DIR}/include
        )

target_link_libraries(${CONFIG_TEST_NAME}
        PRIVATE
        sem_types
        re_node_manager_lib
        gtest
        gtest_main
        )


