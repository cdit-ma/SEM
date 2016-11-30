cmake_minimum_required (VERSION 2.6)

#Use C++11
set(CMAKE_CXX_STANDARD 11)
#Set our module path
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/../../client/cmake/modules/")

set(PROJ_NAME zmq_library)
project (${PROJ_NAME})

find_package(Protobuf REQUIRED)
include_directories(${PROJ_NAME} ${PROTOBUF_INCLUDE_DIRS})

find_package(ZeroMQ REQUIRED)
include_directories(${PROJ_NAME} ${ZeroMQ_INCLUDE_DIRS})

protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS message.proto)

set(SOURCE
	${PROTO_SRCS}
	${CMAKE_CURRENT_SOURCE_DIR}/zmqtxmessage.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/zmqrxmessage.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/convert.cpp
	)

set(HEADERS
	${PROTO_HDRS}
	${CMAKE_CURRENT_SOURCE_DIR}/zmqtxmessage.h
	${CMAKE_CURRENT_SOURCE_DIR}/zmqrxmessage.h
	${CMAKE_CURRENT_SOURCE_DIR}/convert.h
	)

add_library(${PROJ_NAME} ${SOURCE} ${HEADERS})


target_link_libraries(${PROJ_NAME} ${PROTOBUF_LIBRARIES})
target_link_libraries(${PROJ_NAME} ${ZeroMQ_LIBRARIES})
target_link_libraries(${PROJ_NAME} pthread dl)


