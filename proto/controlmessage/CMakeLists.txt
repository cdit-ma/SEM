cmake_minimum_required (VERSION 2.6)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

#Set our project names
set(PROJ_NAME re_common_proto_control)

if(NOT PROTOBUF_FOUND)
	# Find package Protobuf
	find_package(Protobuf REQUIRED)
endif()

set(SOURCE
	${CMAKE_CURRENT_SOURCE_DIR}/helper.cpp
)

#Generate out protobuf for control
protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS ${CMAKE_CURRENT_SOURCE_DIR}/controlmessage.proto)

add_library(${PROJ_NAME} STATIC ${PROTO_SRCS} ${SOURCE} ${PROTO_HDRS} ${HEADERS})

target_compile_definitions(${PROJ_NAME} PUBLIC PROTOBUF_USE_DLLS)

target_include_directories(${PROJ_NAME} PUBLIC ${PROTOBUF_INCLUDE_DIRS})
target_include_directories(${PROJ_NAME} PUBLIC ${RE_COMMON_SOURCE_DIR})
target_include_directories(${PROJ_NAME} PUBLIC ${RE_COMMON_BINARY_DIR})

target_link_libraries(${PROJ_NAME} PUBLIC ${PROTOBUF_LIBRARIES})
