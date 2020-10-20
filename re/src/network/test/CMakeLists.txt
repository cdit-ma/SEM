project(test_re_network VERSION ${PROJECT_VERSION})

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/test)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}/test)

add_executable(${PROJECT_NAME} "")
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)

target_sources(${PROJECT_NAME} PRIVATE
        # TODO: Add pub sub test
        ${CMAKE_CURRENT_SOURCE_DIR}/reqrep.cpp
        )
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../..)
target_include_directories(${PROJECT_NAME} PRIVATE ${GOOGLE_TEST_DIR}/include)

target_link_libraries(${PROJECT_NAME} PRIVATE sem_types)
target_link_libraries(${PROJECT_NAME} PRIVATE gtest)
target_link_libraries(${PROJECT_NAME} PRIVATE gtest_main)
target_link_libraries(${PROJECT_NAME} PRIVATE re_network)
target_link_libraries(${PROJECT_NAME} PUBLIC re_common_proto_modelevent)
