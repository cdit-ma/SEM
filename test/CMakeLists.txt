#Set output directory to be in the source code directory for test. This allows easy directory navigation to load resource files
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

#The MEDEA tests must be left in the directory bin containing the test cases. All paths for test cases
#Are relative to this location. On windows may require Qt5_DIR/bin in path
add_subdirectory("idlparser")
add_subdirectory("modelcontroller")