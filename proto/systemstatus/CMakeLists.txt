cmake_minimum_required (VERSION 2.6)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

#Set our project names
set(PROJ_NAME re_common_proto_systemstatus)

#Find protobuf
find_package(Protobuf REQUIRED)

#Generate out protobuf for control
protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS ${CMAKE_CURRENT_SOURCE_DIR}/systemstatus.proto)

include_directories(${PROTOBUF_INCLUDE_DIRS})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_library(${PROJ_NAME} SHARED ${PROTO_SRCS} ${PROTO_HDRS})

#Link against protobuf
target_link_libraries(${PROJ_NAME} ${PROTOBUF_LIBRARIES})

