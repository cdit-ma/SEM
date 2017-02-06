cmake_minimum_required (VERSION 2.6)


set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/../cmake/modules")

set(PROJ_NAME message_writer)

project (${PROJ_NAME})

find_package(Protobuf REQUIRED)
include_directories(${PROJ_NAME} ${PROTOBUF_INCLUDE_DIRS})

find_package(ZeroMQ REQUIRED)
include_directories(${PROJ_NAME} ${ZeroMQ_INCLUDE_DIRS})

set(SOURCE
    ${CMAKE_CURRENT_SOURCE_DIR}/cachedzmqmessagewriter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/zmqmessagewriter.cpp
	)

set(HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/cachedzmqmessagewriter.h
    ${CMAKE_CURRENT_SOURCE_DIR}/zmqmessagewriter.h
    )

add_library(${PROJ_NAME} SHARED ${SOURCE} ${HEADERS})

#Link against our required libraries
target_link_libraries(${PROJ_NAME} ${PROTOBUF_LIBRARIES})
target_link_libraries(${PROJ_NAME} ${ZeroMQ_LIBRARIES})
