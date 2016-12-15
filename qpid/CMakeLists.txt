cmake_minimum_required (VERSION 2.6)

#Use C++11
set(CMAKE_CXX_STANDARD 11)
#Set our module path
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules/")

set(RUNTIME_INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/../includes/)
include_directories(${PROJ_NAME} ${RUNTIME_INCLUDE}/)

set(PROJ_NAME qpid_library)
project (${PROJ_NAME})

#add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/../proto" "${CMAKE_CURRENT_BINARY_DIR}/proto/")

find_package(Qpid REQUIRED)
include_directories(${PROJ_NAME} ${Qpid_INCLUDE_DIRS})

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
	#${CMAKE_CURRENT_SOURCE_DIR}/../proto/messageconvert.cpp
	#${CMAKE_CURRENT_SOURCE_DIR}/../proto/message.pb.cc
	#${CMAKE_CURRENT_SOURCE_DIR}/../proto/message.pb.h
	)

set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} " -fvisibility=hidden")

add_library(${PROJ_NAME} SHARED ${SOURCE} ${HEADERS})

message(${Qpid_INCLUDE_DIRS})

target_link_libraries(${PROJ_NAME} ${Qpid_LIBRARIES})
target_link_libraries(${PROJ_NAME} ${Proton_LIBRARIES})
target_link_libraries(${PROJ_NAME} pthread dl)
target_link_libraries(${PROJ_NAME} proto_library)
