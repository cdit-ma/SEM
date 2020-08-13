add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/protoreceiver")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/protowriter")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/protoreplier")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/protorequester")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/protoregister")

add_library(zmq_utils INTERFACE)
target_sources(zmq_utils 
		INTERFACE 
		${CMAKE_CURRENT_SOURCE_DIR}/zmqutils.hpp
		)
target_include_directories(zmq_utils INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(zmq_utils INTERFACE cppzmq protobuf::libprotobuf)