
cmake_minimum_required (VERSION 2.6)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(LOGAN_SRC_PATH "${CMAKE_CURRENT_SOURCE_DIR}/src")

set(LOGAN_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(LOGAN_BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}/src")

set(LOGAN_SOURCE_DIR "${LOGAN_SOURCE_DIR}" PARENT_SCOPE)
set(LOGAN_BINARY_DIR "${LOGAN_BINARY_DIR}" PARENT_SCOPE)


get_directory_property(IS_SUB_REPO PARENT_DIRECTORY)

# Only set this if we aren't a sub repo
if(NOT IS_SUB_REPO)
	#Use CCache where possible
	find_program(CCACHE_FOUND ccache)
	if(CCACHE_FOUND)
		message("FOUND CCACHE")
		set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ccache)
		set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK ccache)
	endif(CCACHE_FOUND)

	if(APPLE)
		set(CMAKE_MACOSX_RPATH 1)
	endif(APPLE)

	#Set the projects RPATH to look in the executable directory
	set(CMAKE_INSTALL_RPATH "@loader_path;$ORIGIN;@loader_path/../lib/;$ORIGIN/../lib/")
	
	set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake_modules")

	#Set the binary and library locations
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin")
	set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin")
	set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin")

	if(MSVC)
		SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_CURRENT_SOURCE_DIR}/bin")
		SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE "${CMAKE_CURRENT_SOURCE_DIR}/bin")
		SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${CMAKE_CURRENT_SOURCE_DIR}/bin")
	endif()
endif()

SET(LOGAN_VERSION "3.2.6")

#Add Options for building projects, default to build everything
option(BUILD_SERVER "Build logan_server" ON)
option(BUILD_CLIENT "Build logan_client" ON)
option(DISABLE_MODEL_LOGGING "Disable model logging tables in logan_server")
option(DISABLE_HARDWARE_LOGGING "Disable hardware logging tables in logan_server")

#Build the source
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/re_common")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/src")
