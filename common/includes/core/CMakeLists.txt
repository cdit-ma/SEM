cmake_minimum_required (VERSION 2.6)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

#Set the RPATH for Projects
set(CMAKE_MACOSX_RPATH 1)
set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
set(CMAKE_INSTALL_RPATH "@executable_path;@executable_path/../lib;$ORIGIN;$ORIGIN/../lib")

#Get the RE_PATH
set(RE_PATH "$ENV{RE_PATH}")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${RE_PATH}/cmake/modules")

#Set the binary and library locations
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/../../bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/../../lib")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/../../lib")

#Set our project names
set(CORE_NAME re_core)
set(PROTO_CONTROL_NAME re_proto_control)

#Add the re_node_manager to the directory listings
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/nodemanager")

#Find protobuf
find_package(Protobuf REQUIRED)
#Find ZMQ
find_package(ZeroMQ REQUIRED)

#Generate out protobuf for control
protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS ${CMAKE_CURRENT_SOURCE_DIR}/controlmessage.proto)

set(CORE_SOURCE
    ${CMAKE_CURRENT_SOURCE_DIR}/activatable.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/component.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/nodecontainer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/periodiceventport.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/basemessage.cpp
	)

set(CORE_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/activatable.h
    ${CMAKE_CURRENT_SOURCE_DIR}/component.h
    ${CMAKE_CURRENT_SOURCE_DIR}/eventport.h
    ${CMAKE_CURRENT_SOURCE_DIR}/nodecontainer.h
    ${CMAKE_CURRENT_SOURCE_DIR}/periodiceventport.h
    ${CMAKE_CURRENT_SOURCE_DIR}/basemessage.h
	)

set(PROTO_CONTROL_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/translate.cpp
    )

set(PROTO_CONTROL_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/translate.h
	)

include_directories(${PROTOBUF_INCLUDE_DIRS})
include_directories(${ZeroMQ_INCLUDE_DIRS})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

#include the current binary directory to re_node_manager as it requires the .h file from the pb generated files
target_include_directories(re_node_manager PUBLIC ${CMAKE_CURRENT_BINARY_DIR})

if (MSVC)
	#Visual studio needs to be told to build in Multithreaded Static mode
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
endif()

add_library(${PROTO_CONTROL_NAME} SHARED ${PROTO_CONTROL_SOURCES} ${PROTO_CONTROL_HEADERS} ${PROTO_SRCS} ${PROTO_HDRS})
add_library(${CORE_NAME} SHARED ${CORE_SOURCE} ${CORE_HEADERS})


if(UNIX)
	target_link_libraries(${CORE_NAME} pthread)
endif(UNIX)

#Link re_proto_control against protobuf
target_link_libraries(${PROTO_CONTROL_NAME} ${PROTOBUF_LIBRARIES})

#Link re_core against re_proto_control
target_link_libraries(${CORE_NAME} ${PROTO_CONTROL_NAME})

