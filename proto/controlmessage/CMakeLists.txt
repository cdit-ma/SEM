cmake_minimum_required (VERSION 2.6)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS true)

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

add_library(${PROJ_NAME} SHARED ${PROTO_SRCS} ${SOURCE} ${PROTO_HDRS} ${HEADERS})

target_include_directories(${PROJ_NAME} PRIVATE ${RE_SRC_PATH})
target_include_directories(${PROJ_NAME} PRIVATE ${PROTOBUF_INCLUDE_DIRS})
target_include_directories(${PROJ_NAME} PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
target_include_directories(${PROJ_NAME} PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/../../../)

if (MSVC)
	#Windows requires protobuf in DLLs
    target_compile_definitions(${PROJ_NAME} PRIVATE -DPROTOBUF_USE_DLLS)
	#Visual studio needs to be told to build in Multithreaded Dynamically Linked mode
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MD")
endif()

#Link against protobuf
target_link_libraries(${PROJ_NAME} ${PROTOBUF_LIBRARIES})
