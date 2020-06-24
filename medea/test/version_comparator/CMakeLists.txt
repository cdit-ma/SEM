set(PROJ_NAME test_versioncomparator)
find_package(Qt5 COMPONENTS Test REQUIRED)

add_executable(${PROJ_NAME} "")

target_sources(${PROJ_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/tests.cpp
)

target_link_libraries(${PROJ_NAME}
        PUBLIC
        Qt5::Test
        medea_modelcontroller_lib
        sem_version)

add_test(${PROJ_NAME} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${PROJ_NAME})
