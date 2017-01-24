#Get the RE_PATH
set(RE_PATH "$ENV{RE_PATH}")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${RE_PATH}/cmake/modules")

set(PROJ_NAME ports_message_proto)
project (${PROJ_NAME})

#Find re_core library
find_library(RE_CORE_LIBRARIES re_core "${RE_PATH}/lib")

#Find protobuf
find_package(Protobuf REQUIRED)

protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS message.proto)

set(SOURCE
	${PROTO_SRCS}
	${CMAKE_CURRENT_SOURCE_DIR}/convert.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/../message.cpp
	)

set(HEADERS
	${PROTO_HDRS}
	${CMAKE_CURRENT_SOURCE_DIR}/convert.h
	)

add_library(${PROJ_NAME} SHARED ${SOURCE} ${HEADERS})

include_directories(${PROTOBUF_INCLUDE_DIRS})
include_directories(${RE_PATH}/include)
#include the current binary directory to enable linking of pb.h
include_directories(${CMAKE_CURRENT_BINARY_DIR})

target_link_libraries(${PROJ_NAME} ${RE_CORE_LIBRARIES})
target_link_libraries(${PROJ_NAME} ${PROTOBUF_LIBRARIES})
target_link_libraries(${PROJ_NAME} ports_message)


