if(BUILD_CLIENT)
	message(STATUS "Building logan_client")
	add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/client")
endif(BUILD_CLIENT)

if(BUILD_SERVER)
	message(STATUS "Building logan_server")
	add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/server")
endif(BUILD_SERVER)

if(BUILD_AGGREGATIONSERVER)
	message(STATUS "Building aggregation_server")
	add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/aggregationserver")
endif(BUILD_AGGREGATIONSERVER)