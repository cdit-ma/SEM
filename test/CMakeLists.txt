#Set output directory to be in the source code directory for test. This allows easy directory navigation to load resource files
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/test)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/test)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}/test)

set(GOOGLE_TEST_DIR ${CMAKE_CURRENT_SOURCE_DIR}/googletest/googletest)
if(MSVC)
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
endif(MSVC)

set(RE_TEST_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}")

add_subdirectory("${GOOGLE_TEST_DIR}")

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/common)
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/core")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/logan")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/workers")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/middleware")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/re_common")

if(BUILD_REGEN_TEST)
	message(STATUS "Building re_gen tests")
	#Build the test
	add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/re_gen")
endif(BUILD_REGEN_TEST)