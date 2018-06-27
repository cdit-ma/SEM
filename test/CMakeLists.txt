#Set output directory to be in the source code directory for test. This allows easy directory navigation to load resource files


set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/test)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}/test)

set(GOOGLE_TEST_DIR ${CMAKE_CURRENT_SOURCE_DIR}/googletest/googletest)
#The MEDEA tests must be left in the directory bin containing the test cases. All paths for test cases
#Are relative to this location. On windows may require Qt5_DIR/bin in path
if(MSVC)
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
endif(MSVC)

add_subdirectory("${GOOGLE_TEST_DIR}")

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/core")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/workers")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/middleware")
#add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/re_gen")


