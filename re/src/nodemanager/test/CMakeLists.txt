
set(SEM_BINARY_DIRECTORY_PATH ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/build_time_variables.hpp.in
        ${CMAKE_CURRENT_BINARY_DIR}/build_time_variables.hpp
        @ONLY
        )

add_library(build_time_variables INTERFACE)
target_compile_features(build_time_variables INTERFACE cxx_std_17)
target_sources(build_time_variables
        INTERFACE
        ${CMAKE_CURRENT_BINARY_DIR}/build_time_variables.hpp
        )
target_include_directories(build_time_variables
        INTERFACE
        ${CMAKE_CURRENT_BINARY_DIR}
        )

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
        nodemanager_config
        gtest
        gtest_main
        )

add_executable(epm_registry_test)
target_compile_features(epm_registry_test PRIVATE cxx_std_17)

target_sources(epm_registry_test
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/epm_registry.cpp)

target_include_directories(epm_registry_test
        PRIVATE
        ${GOOGLE_TEST_DIR}/include
        )

add_dependencies(epm_registry_test experiment_process_manager)

target_link_libraries(epm_registry_test
        PRIVATE
        sem_types
        epm_registry
        gtest
        gtest_main
        build_time_variables
        )