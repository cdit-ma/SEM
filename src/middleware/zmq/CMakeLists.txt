cmake_minimum_required (VERSION 2.6)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

#Get the RE_PATH
set(RE_PATH "$ENV{RE_PATH}")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "$ENV{CMAKE_MODULE_PATH}")

#Find ZeroMQ
find_package(ZMQ)

set(PROJ_NAME zmq_helper)
project (${PROJ_NAME})

if(NOT ZMQ_FOUND)
    message(STATUS "Not Found ZMQ - Not building ZMQ Middleware library")
    return()
endif()

set(SOURCE
    ${CMAKE_CURRENT_SOURCE_DIR}/zmqhelper.cpp
	)

set(HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/zmqhelper.h
    )

if (MSVC)
    #Windows requires protobuf in DLLs
    add_definitions(-DPROTOBUF_USE_DLLS)
    #Visual studio needs to be told to build in Multithreaded Dynamically Linked mode
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MD")
endif()

add_library(${PROJ_NAME} SHARED ${SOURCE} ${HEADERS})

include_directories("${RE_PATH}/includes")
include_directories(${ZMQ_INCLUDE_DIRS})

target_link_libraries(${PROJ_NAME} ${ZMQ_LIBRARIES})

