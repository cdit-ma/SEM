set(PROJ_NAME test_memoryworker)
project(${PROJ_NAME} VERSION ${PROJECT_VERSION})


add_executable(${PROJ_NAME} "")
target_compile_features(${PROJ_NAME} PRIVATE cxx_std_11)

target_sources(${PROJ_NAME} PRIVATE
    # Sources
    ${CMAKE_CURRENT_SOURCE_DIR}/tests.cpp
)

target_link_libraries(${PROJ_NAME} PRIVATE memory_worker)
target_link_libraries(${PROJ_NAME} PRIVATE gtest)
target_link_libraries(${PROJ_NAME} PRIVATE gtest_main)