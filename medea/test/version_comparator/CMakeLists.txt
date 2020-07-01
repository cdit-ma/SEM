find_package(Qt5 COMPONENTS Test REQUIRED)

add_executable(test_versioncomparator)

target_sources(test_versioncomparator PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/tests.cpp
        )

target_link_libraries(test_versioncomparator
        PRIVATE
        Qt5::Test
        medea_modelcontroller_lib
        sem_version
        )

add_test(test_versioncomparator ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test_versioncomparator)
