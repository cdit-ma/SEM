cmake_minimum_required (VERSION 2.6)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

#Get the RE_PATH
set(RE_PATH "$ENV{RE_PATH}")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${RE_PATH}/cmake/modules")

set(PROJ_NAME zmqprotowriter)

project (${PROJ_NAME})

find_package(Protobuf REQUIRED)
include_directories(${PROJ_NAME} ${PROTOBUF_INCLUDE_DIRS})

find_package(ZMQ REQUIRED)
include_directories(${PROJ_NAME} ${ZMQ_INCLUDE_DIRS})

#Need boost system and filesystem for crossplatform temp file creation.
find_package(Boost 1.36.0 COMPONENTS filesystem system REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})

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
target_link_libraries(${PROJ_NAME} ${ZMQ_LIBRARIES})
target_link_libraries(${PROJ_NAME} ${Boost_FILESYSTEM_LIBRARY} ${Boost_SYSTEM_LIBRARY})
