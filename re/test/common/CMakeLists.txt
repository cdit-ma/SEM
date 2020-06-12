project(test_common)

# Currently a header only library rather than a static library
add_library(${PROJECT_NAME} INTERFACE)

target_sources(${PROJECT_NAME} INTERFACE
        # Sources
        ${CMAKE_CURRENT_SOURCE_DIR}/common.h
        ${CMAKE_CURRENT_SOURCE_DIR}/testtypes.h
        )
target_include_directories(${PROJECT_NAME} INTERFACE ${GOOGLE_TEST_DIR}/include)
target_link_libraries(${PROJECT_NAME} INTERFACE gtest)
target_link_libraries(${PROJECT_NAME} INTERFACE gtest_main)