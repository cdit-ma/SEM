cdit_ma_generate_protobuf_cpp(PROTO_SRCS PROTO_HDRS testmessage.proto)

add_library(sem_types_proto_testmessage STATIC)

target_sources(sem_types_proto_testmessage
        PRIVATE
        ${PROTO_SRCS}
        ${PROTO_HDRS}
        )

# Include Directories
target_include_directories(sem_types_proto_testmessage
        PUBLIC
        ${CMAKE_CURRENT_BINARY_DIR}
        )

# Link Libraries
target_link_libraries(sem_types_proto_testmessage
        PUBLIC
        protobuf::libprotobuf
        )