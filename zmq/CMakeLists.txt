cmake_minimum_required (VERSION 2.6)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

#Set our module path
set(CDITMA_RUNTIME_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/../common")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CDITMA_RUNTIME_ROOT}/cmake/modules")

set(PROJ_NAME zmq_library)
project (${PROJ_NAME})

include_directories(${PROJ_NAME} "${CDITMA_RUNTIME_ROOT}/includes")

find_package(ZeroMQ REQUIRED)
include_directories(${PROJ_NAME} ${ZeroMQ_INCLUDE_DIRS})

set(SOURCE
	${PROTO_SRCS}
	${CMAKE_CURRENT_SOURCE_DIR}/zmqtxmessage.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/zmqrxmessage.cpp
	)

set(HEADERS
	${PROTO_HDRS}
	${CMAKE_CURRENT_SOURCE_DIR}/zmqtxmessage.h
	${CMAKE_CURRENT_SOURCE_DIR}/zmqrxmessage.h
	${CMAKE_CURRENT_SOURCE_DIR}/../proto/messageconvert.h
	)

#Set symbols in dynamic library to be default hidden
set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} " -fvisibility=hidden")

add_library(${PROJ_NAME} SHARED ${SOURCE} ${HEADERS})

target_link_libraries(${PROJ_NAME} ${ZeroMQ_LIBRARIES})
target_link_libraries(${PROJ_NAME} pthread)
target_link_libraries(${PROJ_NAME} proto_library)
