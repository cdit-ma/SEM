#Get the RE_PATH
set(RE_PATH "$ENV{RE_PATH}")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${RE_PATH}/cmake/modules")

set(PROJ_NAME qpid_message)
project (${PROJ_NAME})

#Find re_core library
find_library(RE_CORE_LIBRARIES re_core "${RE_PATH}/lib")

#Find Qpid
find_package(Qpid REQUIRED)

set(SOURCE
	${CMAKE_CURRENT_SOURCE_DIR}/libportexports.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/rx.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/tx.cpp
	)

set(HEADERS
	${CMAKE_CURRENT_SOURCE_DIR}/rx.h
	${CMAKE_CURRENT_SOURCE_DIR}/tx.h
	)

include_directories("${RE_PATH}/includes")
include_directories(${ZeroMQ_INCLUDE_DIRS})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../proto)

#Set symbols in dynamic library to be default hidden
#set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} " -fvisibility=hidden")

add_library(${PROJ_NAME} MODULE ${SOURCE} ${HEADERS})

target_link_libraries(${PROJ_NAME} ${RE_CORE_LIBRARIES})
target_link_libraries(${PROJ_NAME} ${Qpid_LIBRARIES})

target_link_libraries(${PROJ_NAME} pthread)
target_link_libraries(${PROJ_NAME} proto_message)
target_link_libraries(${PROJ_NAME} datatypes_message)