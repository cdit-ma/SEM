cmake_minimum_required (VERSION 2.6)

#Use C++11
set(CMAKE_CXX_STANDARD 11)
#Set our module path
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules/")

set(RUNTIME_INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/../includes/)
include_directories(${PROJ_NAME} ${RUNTIME_INCLUDE}/)

set(PROJ_NAME qpid_library)
project (${PROJ_NAME})

find_package(Protobuf REQUIRED)
include_directories(${PROJ_NAME} ${PROTOBUF_INCLUDE_DIRS})

find_package(Qpid REQUIRED)
include_directories(${PROJ_NAME} ${Qpid_INCLUDE_DIRS})


#find_package(Proton REQUIRED)
#include_directories(${PROJ_NAME} ${Proton_INCLUDE_DIRS})

#protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS message.proto)

set(SOURCE
	${PROTO_SRCS}
	${CMAKE_CURRENT_SOURCE_DIR}/qpidtxmessage.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/qpidrxmessage.cpp
	)

set(HEADERS
	${PROTO_HDRS}
	${CMAKE_CURRENT_SOURCE_DIR}/qpidtxmessage.h
	${CMAKE_CURRENT_SOURCE_DIR}/qpidrxmessage.h
	${CMAKE_CURRENT_SOURCE_DIR}/../proto/messageconvert.h
	${CMAKE_CURRENT_SOURCE_DIR}/../proto/messageconvert.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/../proto/message.pb.cc
	${CMAKE_CURRENT_SOURCE_DIR}/../proto/message.pb.h
	)

add_library(${PROJ_NAME} SHARED ${SOURCE} ${HEADERS})

message(${Qpid_INCLUDE_DIRS})

target_link_libraries(${PROJ_NAME} ${PROTOBUF_LIBRARIES})
target_link_libraries(${PROJ_NAME} ${Qpid_LIBRARIES})
target_link_libraries(${PROJ_NAME} ${Proton_LIBRARIES})
target_link_libraries(${PROJ_NAME} pthread dl)

