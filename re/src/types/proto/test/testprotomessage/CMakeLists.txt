cdit_ma_generate_protobuf_cpp(PROTO_SRCS PROTO_HDRS ${CMAKE_CURRENT_SOURCE_DIR}/testmessage.proto)

add_library(re_types_proto_testmessage STATIC)

target_sources(re_types_proto_testmessage
        PRIVATE
        ${PROTO_SRCS}
        ${PROTO_HDRS}
        )

# Include Directories
target_include_directories(re_types_proto_testmessage
        PUBLIC
        ${CMAKE_CURRENT_BINARY_DIR}
        )

# Link Libraries
target_link_libraries(re_types_proto_testmessage
        PUBLIC
        protobuf::libprotobuf
        )