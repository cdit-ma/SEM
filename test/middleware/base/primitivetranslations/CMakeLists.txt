set(PROJ_NAME test_primitivetranslations)
project(${PROJ_NAME})

add_executable(${PROJ_NAME})

target_sources(${PROJ_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/tests.cpp
)

target_include_directories(${PROJ_NAME} PRIVATE ${RE_SOURCE_PATH})
target_link_libraries(${PROJ_NAME} PRIVATE gtest)
target_link_libraries(${PROJ_NAME} PRIVATE gtest_main)