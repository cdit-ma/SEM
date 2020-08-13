cdit_ma_generate_protobuf_cpp(PROTO_SRCS PROTO_HDRS ${CMAKE_CURRENT_SOURCE_DIR}/aggregationmessage.proto)

add_library(re_common_proto_aggregation_message)
target_compile_features(re_common_proto_aggregation_message PRIVATE cxx_std_17)

target_sources(re_common_proto_aggregation_message
		PRIVATE
		${PROTO_HDRS}
		${PROTO_SRCS}
		)

# Include Directories
target_include_directories(re_common_proto_aggregation_message 
		PUBLIC
		${CMAKE_CURRENT_BINARY_DIR}
		)

# Link Libraries 
target_link_libraries(re_common_proto_aggregation_message
		PUBLIC
		protobuf::libprotobuf
		)