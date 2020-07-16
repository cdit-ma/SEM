protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS ${CMAKE_CURRENT_SOURCE_DIR}/environmentcontrol.proto)

add_library(re_common_proto_environmentcontrol)
target_compile_features(re_common_proto_environmentcontrol PRIVATE cxx_std_11)

target_sources(re_common_proto_environmentcontrol
        PRIVATE
        ${PROTO_SRCS}
        ${PROTO_HDRS}
        )
target_include_directories(re_common_proto_environmentcontrol
        PUBLIC
        ${CMAKE_CURRENT_BINARY_DIR}
        )
target_link_libraries(re_common_proto_environmentcontrol
        PUBLIC
        protobuf::libprotobuf
        )
