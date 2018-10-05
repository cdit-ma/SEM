cmake_minimum_required (VERSION 2.6)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

set(PROJ_NAME zmq_protoreceiver)
project (${PROJ_NAME})

set(SOURCE
    ${CMAKE_CURRENT_SOURCE_DIR}/protoreceiver.cpp
	)

set(HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/protoreceiver.h
    )

add_library(${PROJ_NAME} SHARED ${SOURCE} ${HEADERS})

target_link_libraries(${PROJ_NAME} zmq_protoregister)