set(PROJ_NAME re_common_proto_control)
project(${PROJ_NAME})

find_package(Protobuf REQUIRED)

set(SOURCE
	${CMAKE_CURRENT_SOURCE_DIR}/helper.cpp
)

# Generate .proto file
protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS ${CMAKE_CURRENT_SOURCE_DIR}/controlmessage.proto)

add_library(${PROJ_NAME} STATIC ${SOURCE} ${HEADERS} ${PROTO_SRCS} ${PROTO_HDRS})

# Compile Definitions
target_compile_definitions(${PROJ_NAME} PUBLIC PROTOBUF_USE_DLLS)

# Include Directories
target_include_directories(${PROJ_NAME} PUBLIC ${PROTOBUF_INCLUDE_DIRS})
target_include_directories(${PROJ_NAME} PUBLIC ${RE_COMMON_SOURCE_DIR})
target_include_directories(${PROJ_NAME} PUBLIC ${RE_COMMON_BINARY_DIR})

# Link Libraries 
target_link_libraries(${PROJ_NAME} PUBLIC ${PROTOBUF_LIBRARIES})