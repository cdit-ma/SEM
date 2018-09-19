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
	)

set(HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/environmentrequester.h
    )

add_library(${PROJ_NAME} SHARED ${SOURCE} ${HEADERS})

if (MSVC)
	#Visual studio needs to be told to build in Multithreaded Dynamically Linked mode
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MD")
    target_compile_definitions(${PROJ_NAME} PRIVATE -DPROTOBUF_USE_DLLS)    
endif()

target_include_directories(${PROJ_NAME} PRIVATE ${PROTOBUF_INCLUDE_DIRS})
target_include_directories(${PROJ_NAME} PRIVATE ${ZMQ_INCLUDE_DIRS})
target_include_directories(${PROJ_NAME} PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/../../proto/controlmessage)

target_link_libraries(${PROJ_NAME} re_common_proto_control)
target_link_libraries(${PROJ_NAME} zmq_protorequester)
target_link_libraries(${PROJ_NAME} ${ZMQ_LIBRARIES})
