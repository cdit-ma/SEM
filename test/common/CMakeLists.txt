project(test_common)

add_library(${PROJECT_NAME} STATIC "")
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_11)

target_sources(${PROJECT_NAME} PRIVATE
        # Sources
        ${CMAKE_CURRENT_SOURCE_DIR}/common.h
        ${CMAKE_CURRENT_SOURCE_DIR}/testtypes.h
        )
target_include_directories(${PROJECT_NAME} PRIVATE ${GOOGLE_TEST_DIR}/include)
target_link_libraries(${PROJECT_NAME} PRIVATE cpu_worker)
target_link_libraries(${PROJECT_NAME} PRIVATE gtest)
target_link_libraries(${PROJECT_NAME} PRIVATE gtest_main)