cmake_minimum_required (VERSION 2.6)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

#get the cmake module path
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "$ENV{CMAKE_MODULE_PATH}/cmake/modules")

set(PROJ_NAME zmq_monitor)

project (${PROJ_NAME})

find_package(ZMQ REQUIRED)
include_directories(${ZMQ_INCLUDE_DIRS})

set(SOURCE
    ${CMAKE_CURRENT_SOURCE_DIR}/monitor.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/monitorable.cpp
	)

set(HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/monitor.h
    ${CMAKE_CURRENT_SOURCE_DIR}/monitorable.h
    )

add_library(${PROJ_NAME} SHARED ${SOURCE} ${HEADERS})

if (MSVC)
	#Windows requires protobuf in DLLs
    add_definitions(-DPROTOBUF_USE_DLLS)
	#Visual studio needs to be told to build in Multithreaded Dynamically Linked mode
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MD")
endif()

#Link against our required libraries
target_link_libraries(${PROJ_NAME} ${ZMQ_LIBRARIES})
