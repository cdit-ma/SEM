cmake_minimum_required (VERSION 2.6)

#Use C++11
set(CMAKE_CXX_STANDARD 11)
#Set our module path
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules/")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/")

set(PROJ_NAME logan_server)

project (${PROJ_NAME})

find_package(Protobuf REQUIRED)
include_directories(${PROJ_NAME} ${PROTOBUF_INCLUDE_DIRS})

#Link against boost dynamically
add_definitions(-DBOOST_ALL_DYN_LINK)
#Need boost > 1.30.0 system and filesystem for crossplatform temp file creation.
find_package(Boost 1.30.0 COMPONENTS program_options REQUIRED)
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
	#Windows requires protobuf in DLLs
    add_definitions(-DPROTOBUF_USE_DLLS)
else()
	#Unix needs to include pthread
	target_link_libraries(${PROJ_NAME} pthread dl)
endif()

#include the current binary directory to re_node_manager as it requires the .h file from the pb generated files
target_include_directories(${PROJ_NAME} PUBLIC ${CMAKE_CURRENT_BINARY_DIR})

#Link against our Protobuf source/headers
target_link_libraries(${PROJ_NAME} re_common_proto_systemstatus)
target_link_libraries(${PROJ_NAME} re_common_proto_modelevent)
target_link_libraries(${PROJ_NAME} zmq_protoreceiver)
target_link_libraries(${PROJ_NAME} zmq_registrant)


#Link against our required libraries
target_link_libraries(${PROJ_NAME} ${PROTOBUF_LIBRARIES})
target_link_libraries(${PROJ_NAME} ${ZeroMQ_LIBRARIES})
target_link_libraries(${PROJ_NAME} ${Boost_PROGRAM_OPTIONS_LIBRARY})
