project(test_representation VERSION ${PROJECT_VERSION})

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/test)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}/test)

add_executable(${PROJECT_NAME} "")
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_BINARY_DIR}/../..)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_BINARY_DIR}/..)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../..)
# See https://github.com/vector-of-bool/cmrc
include(${CMAKE_MODULE_PATH}/CMakeRC.cmake)
cmrc_add_resource_library(
        representation_test_models
        models/complex_replication.graphml
        models/model.graphml
        models/RE414.graphml
        models/RE414_uneven_depth.graphml
        models/simple_replication.graphml
        models/trigger_test.graphml
)

target_sources(${PROJECT_NAME} PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/test.cpp
        )

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../..)
target_include_directories(${PROJECT_NAME} PRIVATE ${GOOGLE_TEST_DIR}/include)

target_link_libraries(${PROJECT_NAME} PRIVATE gtest)
target_link_libraries(${PROJECT_NAME} PRIVATE gtest_main)
target_link_libraries(${PROJECT_NAME} PUBLIC experiment_definition)
target_link_libraries(${PROJECT_NAME} PUBLIC representation_test_models)
target_link_libraries(${PROJECT_NAME} PUBLIC experiment_definition_protocol)
