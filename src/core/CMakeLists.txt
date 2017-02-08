cmake_minimum_required (VERSION 2.6)

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
    ${CMAKE_CURRENT_SOURCE_DIR}/periodiceventport.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/basemessage.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/portconfigure.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/eventport.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/modellogger.cpp

	)

set(CORE_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/activatable.h
    ${CMAKE_CURRENT_SOURCE_DIR}/component.h
    ${CMAKE_CURRENT_SOURCE_DIR}/eventport.h
    ${CMAKE_CURRENT_SOURCE_DIR}/periodiceventport.h
    ${CMAKE_CURRENT_SOURCE_DIR}/basemessage.h
    ${CMAKE_CURRENT_SOURCE_DIR}/portconfigure.h   
    ${CMAKE_CURRENT_SOURCE_DIR}/modellogger.h   
     
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

if (MSVC)
	#Visual studio needs to be told to build in Multithreaded Static mode
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
endif()

add_library(${PROTO_CONTROL_NAME} SHARED ${PROTO_CONTROL_SOURCES} ${PROTO_CONTROL_HEADERS} ${PROTO_SRCS} ${PROTO_HDRS})
add_library(${CORE_NAME} SHARED ${CORE_SOURCE} ${CORE_HEADERS})

target_include_directories(re_core PUBLIC ${CMAKE_CURRENT_BINARY_DIR})

message("BINARY DIRECTPRY: " ${CMAKE_CURRENT_BINARY_DIR})
if(UNIX)
	target_link_libraries(${CORE_NAME} pthread)
endif(UNIX)

#Link re_proto_control against protobuf
target_link_libraries(${PROTO_CONTROL_NAME} ${PROTOBUF_LIBRARIES})

#Link re_core against re_proto_control
target_link_libraries(${CORE_NAME} ${PROTO_CONTROL_NAME})
target_link_libraries(${CORE_NAME} zmqprotowriter)
target_link_libraries(${CORE_NAME} re_common_proto_modelevent)

