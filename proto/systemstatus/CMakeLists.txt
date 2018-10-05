cmake_minimum_required (VERSION 2.6)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS true)

#Set our project names
set(PROJ_NAME re_common_proto_systemstatus)

#Find protobuf
find_package(Protobuf REQUIRED)

#Generate out protobuf for control
protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS ${CMAKE_CURRENT_SOURCE_DIR}/systemstatus.proto)

add_library(${PROJ_NAME} STATIC ${PROTO_SRCS} ${PROTO_HDRS})

target_compile_definitions(${PROJ_NAME} PUBLIC PROTOBUF_USE_DLLS)

target_include_directories(${PROJ_NAME} PUBLIC ${PROTOBUF_INCLUDE_DIRS})
target_include_directories(${PROJ_NAME} PUBLIC ${RE_COMMON_BINARY_DIR})

target_link_libraries(${PROJ_NAME} PUBLIC ${PROTOBUF_LIBRARIES})

