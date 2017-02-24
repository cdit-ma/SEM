cmake_minimum_required (VERSION 2.6)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

#Set our project names
set(PROJ_NAME re_core)
project(${PROJ_NAME})
#Add the re_node_manager to the directory listings

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/controlmessage")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/nodemanager")

#Find protobuf
find_package(Protobuf REQUIRED)
#Find ZMQ
find_package(ZMQ REQUIRED)

set(CORE_SOURCE
    ${CMAKE_CURRENT_SOURCE_DIR}/activatable.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/component.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/periodiceventport.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/basemessage.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/portconfigure.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/eventport.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/modellogger.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/attribute.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/attribute.cpp
	)

set(CORE_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/activatable.h
    ${CMAKE_CURRENT_SOURCE_DIR}/component.h
    ${CMAKE_CURRENT_SOURCE_DIR}/eventport.h
    ${CMAKE_CURRENT_SOURCE_DIR}/periodiceventport.h
    ${CMAKE_CURRENT_SOURCE_DIR}/basemessage.h
    ${CMAKE_CURRENT_SOURCE_DIR}/portconfigure.h   
    ${CMAKE_CURRENT_SOURCE_DIR}/modellogger.h   
    ${CMAKE_CURRENT_SOURCE_DIR}/attribute.h
	)

include_directories(${PROTOBUF_INCLUDE_DIRS})
include_directories(${ZMQ_INCLUDE_DIRS})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_library(${PROJ_NAME} SHARED ${CORE_SOURCE} ${CORE_HEADERS})
if (MSVC)
	#Windows requires protobuf in DLLs
    add_definitions(-DPROTOBUF_USE_DLLS)
	#Visual studio needs to be told to build in Multithreaded Dynamically Linked mode
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MD")
elseif(UNIX)
	target_link_libraries(${PROJ_NAME} pthread)
endif(MSVC)


target_include_directories(${PROJ_NAME} PUBLIC ${CMAKE_CURRENT_BINARY_DIR})

#Link re_core against re_proto_control
target_link_libraries(${PROJ_NAME} zmqprotowriter)
target_link_libraries(${PROJ_NAME} re_common_proto_modelevent)
target_link_libraries(${PROJ_NAME} re_proto_control)

