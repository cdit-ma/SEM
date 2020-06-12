set(PROJ_NAME test_execution)
project(${PROJ_NAME} VERSION ${PROJECT_VERSION})

add_executable(${PROJ_NAME} "")
target_compile_features(${PROJ_NAME} PRIVATE cxx_std_17)

target_sources(${PROJ_NAME} PRIVATE
    # Sources
    ${CMAKE_CURRENT_SOURCE_DIR}/execution_test.cpp
)

target_include_directories(${PROJ_NAME} PRIVATE ${RE_COMMON_SOURCE_DIR})

target_link_libraries(${PROJ_NAME} PRIVATE gtest)
target_link_libraries(${PROJ_NAME} PRIVATE gtest_main)