add_library(zmq_protoreceiver STATIC)
target_compile_features(zmq_protoreceiver PRIVATE cxx_std_11)
set_target_properties(zmq_protoreceiver PROPERTIES POSITION_INDEPENDENT_CODE ON)

target_sources(zmq_protoreceiver
		PUBLIC
		${CMAKE_CURRENT_SOURCE_DIR}/protoreceiver.h
		PRIVATE
		${CMAKE_CURRENT_SOURCE_DIR}/protoreceiver.cpp
		)
target_link_libraries(zmq_protoreceiver
		PUBLIC
		zmq_protoregister
		)
target_include_directories(zmq_protoreceiver
		PUBLIC
		${CMAKE_CURRENT_SOURCE_DIR}
		)
