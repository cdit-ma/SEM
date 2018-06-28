cmake_minimum_required (VERSION 2.6)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

#Set our project names
set(PROJ_NAME re_core)
project(${PROJ_NAME})

#Find protobuf
find_package(Protobuf REQUIRED)
find_package(ZMQ REQUIRED)

set(SOURCE
    ${CMAKE_CURRENT_SOURCE_DIR}/activatable.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/behaviourcontainer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/component.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/basemessage.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ports/port.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ports/periodicport.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/modellogger.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/attribute.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/worker.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/translate.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/threadmanager.cpp
	)

set(HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/activatable.h
    ${CMAKE_CURRENT_SOURCE_DIR}/behaviourcontainer.h
    ${CMAKE_CURRENT_SOURCE_DIR}/component.h
    ${CMAKE_CURRENT_SOURCE_DIR}/ports/port.h
    ${CMAKE_CURRENT_SOURCE_DIR}/ports/periodicport.h
    ${CMAKE_CURRENT_SOURCE_DIR}/basemessage.h
    ${CMAKE_CURRENT_SOURCE_DIR}/modellogger.h
    ${CMAKE_CURRENT_SOURCE_DIR}/attribute.h
    ${CMAKE_CURRENT_SOURCE_DIR}/worker.h
    ${CMAKE_CURRENT_SOURCE_DIR}/translate.h
    ${CMAKE_CURRENT_SOURCE_DIR}/threadmanager.h
	)

add_library(${PROJ_NAME} SHARED ${SOURCE} ${HEADERS})

if (MSVC)
	#Windows requires protobuf in DLLs
    target_compile_definitions(${PROJ_NAME} PRIVATE -DPROTOBUF_USE_DLLS)
	#Visual studio needs to be told to build in Multithreaded Dynamically Linked mode
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MD")
elseif(UNIX)
	target_link_libraries(${PROJ_NAME} pthread)
endif(MSVC)

#Include the directories
target_include_directories(${PROJ_NAME} PRIVATE ${PROTOBUF_INCLUDE_DIRS})
target_include_directories(${PROJ_NAME} PRIVATE ${ZMQ_INCLUDE_DIRS})
target_include_directories(${PROJ_NAME} PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/../re_common/)

#target_include_directories(${PROJ_NAME} PRIVATE ${CMAKE_CURRENT_BINARY_DIR})

#Link against other libraries
target_link_libraries(${PROJ_NAME} zmq_protowriter)
target_link_libraries(${PROJ_NAME} re_common_proto_modelevent)
target_link_libraries(${PROJ_NAME} re_common_proto_control)
