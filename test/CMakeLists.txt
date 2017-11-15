#Set output directory to be in the source code directory for test. This allows easy directory navigation to load resource files
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

set(GOOGLE_TEST_DIR ${CMAKE_CURRENT_SOURCE_DIR}/googletest/googletest)
#The MEDEA tests must be left in the directory bin containing the test cases. All paths for test cases
#Are relative to this location. On windows may require Qt5_DIR/bin in path
add_subdirectory("googletest/googletest")
add_subdirectory("core")
#add_subdirectory("workers")
add_subdirectory("middleware")

