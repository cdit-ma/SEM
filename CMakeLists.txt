cmake_minimum_required (VERSION 2.6)

#Set output directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/")

project(logan)

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/re_common")
#Add Options for building projects, default to build everything
option(BUILD_SERVER "Build logan_server" ON)
option(BUILD_CLIENT "Build logan_client" ON)

if(BUILD_CLIENT)
	message(STATUS "Building logan_client")
	add_subdirectory(client)
endif(BUILD_CLIENT)

if(BUILD_SERVER)
	message(STATUS "Building logan_server")
	add_subdirectory(server)
endif(BUILD_SERVER)
