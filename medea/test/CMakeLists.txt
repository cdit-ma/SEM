#Set output directory to be in the source code directory for test. This allows easy directory navigation to load resource files
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test/qttest)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_$<CONFIG> ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/tests/qttest)

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/version_comparator")