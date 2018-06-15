if(BUILD_CLIENT OR BUILD_SERVER)
	message(STATUS "Building re_common")
	add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/re_common")
endif()


if(BUILD_CLIENT)
	message(STATUS "Building logan_client")
	add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/client")
endif(BUILD_CLIENT)

if(BUILD_SERVER)
	message(STATUS "Building logan_server")
	add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/server")
endif(BUILD_SERVER)

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/clientserver")
