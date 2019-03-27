#Set our project names
set(PROJ_NAME re_common_proto_systemevent)
project(${PROJ_NAME})

find_package(Protobuf REQUIRED)

# Generate .proto file
protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS ${CMAKE_CURRENT_SOURCE_DIR}/systemevent.proto)

add_library(${PROJ_NAME} STATIC "")
set_target_properties(${PROJ_NAME} PROPERTIES POSITION_INDEPENDENT_CODE ON)
target_sources(${PROJ_NAME} PRIVATE
        ${PROTO_SRCS}
        ${PROTO_HDRS}
        )

# Compile Definitions
target_compile_definitions(${PROJ_NAME} PUBLIC PROTOBUF_USE_DLLS)

# Include Directories
target_include_directories(${PROJ_NAME} PUBLIC ${PROTOBUF_INCLUDE_DIRS})
target_include_directories(${PROJ_NAME} PUBLIC ${RE_COMMON_SOURCE_DIR})
target_include_directories(${PROJ_NAME} PUBLIC ${RE_COMMON_BINARY_DIR})

# Link Libraries 
target_link_libraries(${PROJ_NAME} PUBLIC ${PROTOBUF_LIBRARIES})