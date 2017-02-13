cmake_minimum_required (VERSION 2.6)
set (CMAKE_CXX_STANDARD 11)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules/")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/")

set(PROJ_NAME logan_server)

project (${PROJ_NAME})

find_package(Protobuf REQUIRED)
include_directories(${PROJ_NAME} ${PROTOBUF_INCLUDE_DIRS})

find_package(Boost 1.36.0 COMPONENTS program_options REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})


set(SOURCE
	${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/logprotohandler.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/sqlitedatabase.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/table.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/tableinsert.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/sqlite3.c
)

set(HEADERS
	${CMAKE_CURRENT_SOURCE_DIR}/logprotohandler.h
	${CMAKE_CURRENT_SOURCE_DIR}/sqlitedatabase.h
	${CMAKE_CURRENT_SOURCE_DIR}/table.h
	${CMAKE_CURRENT_SOURCE_DIR}/tableinsert.h
	${CMAKE_CURRENT_SOURCE_DIR}/sqlite3.h
)

#Link our Server against all the Cpps
add_executable(${PROJ_NAME} ${SOURCE} ${HEADERS})


if (MSVC)
	#Visual studio needs to be told to build in Multithreaded Static mode
        set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
else()
	#Unix needs to include pthread
	target_link_libraries(${PROJ_NAME} pthread dl)
endif()

#Link against our Protobuf source/headers
target_link_libraries(${PROJ_NAME} zmqprotoreceiver)
target_link_libraries(${PROJ_NAME} re_common_proto_systemstatus)
target_link_libraries(${PROJ_NAME} re_common_proto_modelevent)

#include the current binary directory to re_node_manager as it requires the .h file from the pb generated files
target_include_directories(${PROJ_NAME} PUBLIC ${CMAKE_CURRENT_BINARY_DIR})
#Link against our required libraries
target_link_libraries(${PROJ_NAME} ${PROTOBUF_LIBRARIES})
target_link_libraries(${PROJ_NAME} ${ZeroMQ_LIBRARIES})
target_link_libraries(${PROJ_NAME} ${Boost_PROGRAM_OPTIONS_LIBRARY})
#target_link_libraries(${PROJ_NAME} ${Boost_SYSTEM_LIBRARY})
