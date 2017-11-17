cmake_minimum_required (VERSION 2.6)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

#Required for dynamic linking magic
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

#Set the projects RPATH to look in the executable directory
if(APPLE)
	set(CMAKE_MACOSX_RPATH 1)
	set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
	set(CMAKE_INSTALL_RPATH "@loader_path;$ORIGIN;@loader_path/../lib/;$ORIGIN/../lib/")
endif(APPLE)

SET(RE_VERSION "1.3.2")

option(BUILD_TEST "Build testing" OFF)


#Get the RE_PATH
#set(RE_PATH "$ENV{RE_PATH}")
set(RE_SRC_PATH "${CMAKE_CURRENT_SOURCE_DIR}/src")
#Use the local submoduled cmake_modules if we don't have a global one set
set(CMAKE_MODULE_PATH $ENV{CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake_modules")

#Set the binary and library locations
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/lib")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/lib")

if(MSVC)
	SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_CURRENT_SOURCE_DIR}/bin")
	SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE "${CMAKE_CURRENT_SOURCE_DIR}/lib")
	SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${CMAKE_CURRENT_SOURCE_DIR}/lib")
endif()

#Build the source
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/src")

if(BUILD_TEST)
	message(STATUS "Building re tests")
	#Build the test
	add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/test")
endif(BUILD_TEST)
