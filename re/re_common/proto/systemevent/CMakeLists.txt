#Set our project names
set(PROJ_NAME re_common_proto_systemevent)
project(${PROJ_NAME} VERSION ${PROJECT_VERSION})

find_package(Protobuf REQUIRED)

# Generate .proto file
protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS ${CMAKE_CURRENT_SOURCE_DIR}/systemevent.proto)

add_library(${PROJ_NAME} STATIC "")
target_compile_features(${PROJ_NAME} PRIVATE cxx_std_11)
set_target_properties(${PROJ_NAME} PROPERTIES POSITION_INDEPENDENT_CODE ON)
target_compile_definitions(${PROJ_NAME} PUBLIC PROTOBUF_USE_DLLS)

target_sources(${PROJ_NAME}
        PUBLIC
        ${PROTO_HDRS}
        PRIVATE
        ${PROTO_SRCS}
        )
target_include_directories(re_common_proto_systemevent
        PUBLIC
        ${CMAKE_CURRENT_BINARY_DIR}
        )
target_link_libraries(${PROJ_NAME} PUBLIC ${PROTOBUF_LIBRARIES})
