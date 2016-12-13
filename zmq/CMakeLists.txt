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


#add_subdirectory("${PROJECT_SOURCE_DIR}/../proto")

set(SOURCE
	${CMAKE_CURRENT_SOURCE_DIR}/zmqineventport.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/zmqtxmessage.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/zmqrxmessage.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/zmqhelper.cpp
	
	${CMAKE_CURRENT_SOURCE_DIR}/../proto/messageconvert.h
	)

set(HEADERS
	${CMAKE_CURRENT_SOURCE_DIR}/zmqtxmessage.h
	${CMAKE_CURRENT_SOURCE_DIR}/zmqrxmessage.h
	${CMAKE_CURRENT_SOURCE_DIR}/zmqhelper.h
	${CMAKE_CURRENT_SOURCE_DIR}/../proto/messageconvert.h
	${CMAKE_CURRENT_SOURCE_DIR}/../proto/messageconvert.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/../proto/message.pb.cc
	${CMAKE_CURRENT_SOURCE_DIR}/../proto/message.pb.h
)

add_library(${PROJ_NAME} ${SOURCE} ${HEADERS})


target_link_libraries(${PROJ_NAME} ${PROTOBUF_LIBRARIES})
target_link_libraries(${PROJ_NAME} ${ZeroMQ_LIBRARIES})
#target_link_libraries(${PROJ_NAME} proto_library)

target_link_libraries(${PROJ_NAME} pthread dl)


