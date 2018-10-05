cmake_minimum_required (VERSION 2.6)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

set(PROJ_NAME zmq_environmentrequester)
project(${PROJ_NAME})

find_package(ZMQ REQUIRED)
find_package(Protobuf REQUIRED)

set(SOURCE
    ${CMAKE_CURRENT_SOURCE_DIR}/environmentrequester.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/heartbeater.cpp
	)

set(HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/environmentrequester.h
    )

add_library(${PROJ_NAME} SHARED ${SOURCE} ${HEADERS})

target_compile_definitions(${PROJ_NAME} PRIVATE PROTOBUF_USE_DLLS)

target_include_directories(${PROJ_NAME} PRIVATE ${PROTOBUF_INCLUDE_DIRS})
target_include_directories(${PROJ_NAME} PRIVATE ${ZMQ_INCLUDE_DIRS})
target_include_directories(${PROJ_NAME} PRIVATE ${RE_COMMON_SOURCE_DIR})
target_include_directories(${PROJ_NAME} PRIVATE ${RE_COMMON_BINARY_DIR})


target_link_libraries(${PROJ_NAME} re_common_proto_control)
target_link_libraries(${PROJ_NAME} zmq_protorequester)
target_link_libraries(${PROJ_NAME} ${ZMQ_LIBRARIES})
