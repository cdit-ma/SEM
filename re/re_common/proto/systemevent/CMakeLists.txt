find_package(Protobuf REQUIRED)

add_library(re_common_proto_systemevent STATIC "")
# Generate .proto file
protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS ${CMAKE_CURRENT_SOURCE_DIR}/systemevent.proto)

target_compile_features(re_common_proto_systemevent PRIVATE cxx_std_11)
set_target_properties(re_common_proto_systemevent PROPERTIES POSITION_INDEPENDENT_CODE ON)
target_compile_definitions(re_common_proto_systemevent PUBLIC PROTOBUF_USE_DLLS)


target_sources(re_common_proto_systemevent
        PRIVATE
        ${PROTO_HDRS}
        ${PROTO_SRCS}
        )
target_include_directories(re_common_proto_systemevent
        PUBLIC
        ${PROTOBUF_INCLUDE_DIRS}
        ${CMAKE_CURRENT_BINARY_DIR}
        )
target_link_libraries(re_common_proto_systemevent PUBLIC ${PROTOBUF_LIBRARIES})
