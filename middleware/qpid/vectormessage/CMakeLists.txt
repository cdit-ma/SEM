#Get the RE_PATH
set(RE_PATH "$ENV{RE_PATH}")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${RE_PATH}/cmake/modules")

set(PROJ_NAME zmq_vectormessage)
project (${PROJ_NAME})

#Find re_core library
find_library(RE_CORE_LIBRARIES re_core "${RE_PATH}/lib")
find_library(ZMQ_HELPER_LIBRARIES zmq_helper "${RE_PATH}/lib")

#Find ZeroMQ
find_package(ZeroMQ REQUIRED)

set(SOURCE
	${CMAKE_CURRENT_SOURCE_DIR}/libportexports.cpp
	)

include_directories("${RE_PATH}/includes")
include_directories(${ZeroMQ_INCLUDE_DIRS})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../proto)

add_library(${PROJ_NAME} MODULE ${SOURCE} ${HEADERS})

target_link_libraries(${PROJ_NAME} ${RE_CORE_LIBRARIES})
target_link_libraries(${PROJ_NAME} ${ZeroMQ_LIBRARIES})
target_link_libraries(${PROJ_NAME} ${ZMQ_HELPER_LIBRARIES})

target_link_libraries(${PROJ_NAME} pthread)
target_link_libraries(${PROJ_NAME} proto_vectormessage)
target_link_libraries(${PROJ_NAME} datatypes_vectormessage)