find_package(protobuf REQUIRED)
protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS ${CMAKE_CURRENT_SOURCE_DIR}/modelevent.proto)

add_library(re_common_proto_modelevent)
target_compile_features(re_common_proto_modelevent PRIVATE cxx_std_11)
set_target_properties(re_common_proto_modelevent PROPERTIES POSITION_INDEPENDENT_CODE ON)

target_sources(re_common_proto_modelevent PRIVATE
        ${PROTO_HDRS}
        ${PROTO_SRCS}
        )

target_include_directories(re_common_proto_modelevent 
        PUBLIC 
        ${CMAKE_CURRENT_BINARY_DIR}
        )

target_link_libraries(re_common_proto_modelevent 
        PUBLIC
        protobuf::libprotobuf
        )
