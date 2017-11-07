cmake_minimum_required (VERSION 2.6)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS true)

#Set our project names
set(PROJ_NAME re_proto_control)

#Find protobuf
find_package(Protobuf REQUIRED)

#Generate out protobuf for control
protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS ${CMAKE_CURRENT_SOURCE_DIR}/controlmessage.proto)

set(SOURCE
    ${CMAKE_CURRENT_SOURCE_DIR}/translate.cpp
	)

set(HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/translate.h
	)

include_directories(${PROTOBUF_INCLUDE_DIRS})

add_library(${PROJ_NAME} STATIC ${PROTO_SRCS} ${SOURCE} ${PROTO_HDRS} ${HEADERS})

target_include_directories(${PROJ_NAME} PUBLIC ${CMAKE_CURRENT_BINARY_DIR})

if (MSVC)
	#Windows requires protobuf in DLLs
    add_definitions(-DPROTOBUF_USE_DLLS)
	#Visual studio needs to be told to build in Multithreaded Dynamically Linked mode
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MD")
endif()

#Link against protobuf
target_link_libraries(${PROJ_NAME} ${PROTOBUF_LIBRARIES})

