project(test_nodemanager VERSION ${PROJECT_VERSION})

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/test)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}/test)

set(CONFIG_TEST_NAME ${PROJECT_NAME}_config)

add_executable(${CONFIG_TEST_NAME} "")
target_compile_features(${CONFIG_TEST_NAME} PRIVATE cxx_std_17)

target_sources(${CONFIG_TEST_NAME} PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/nodemanagerconfig.cpp
        )
target_include_directories(${CONFIG_TEST_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../..)
target_include_directories(${CONFIG_TEST_NAME} PRIVATE ${GOOGLE_TEST_DIR}/include)

target_link_libraries(${CONFIG_TEST_NAME} PRIVATE sem_types)
target_link_libraries(${CONFIG_TEST_NAME} PRIVATE re_node_manager_lib)
target_link_libraries(${CONFIG_TEST_NAME} PRIVATE gtest)
target_link_libraries(${CONFIG_TEST_NAME} PRIVATE gtest_main)


