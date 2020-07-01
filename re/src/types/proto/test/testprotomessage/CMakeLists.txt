find_package(Protobuf REQUIRED)

protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS ${CMAKE_CURRENT_SOURCE_DIR}/testmessage.proto)

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

if (MSVC)
    #Windows requires protobuf in DLLs
    target_compile_definitions(${PROJ_NAME} PRIVATE -DPROTOBUF_USE_DLLS)
    #Visual studio needs to be told to build in Multithreaded Dynamically Linked mode
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MD")
endif ()

# Link Libraries
target_link_libraries(re_types_proto_testmessage
        PUBLIC
        protobuf::libprotobuf
        )