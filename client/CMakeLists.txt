cmake_minimum_required (VERSION 2.6)

#Use C++11
set(CMAKE_CXX_STANDARD 11)
#Set our module path
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules/")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/")

set(PROJ_NAME logan_client)

project (${PROJ_NAME})

find_package(Protobuf REQUIRED)
include_directories(${PROJ_NAME} ${PROTOBUF_INCLUDE_DIRS})

find_package(SIGAR REQUIRED)
include_directories(${PROJ_NAME} ${SIGAR_INCLUDE_DIRS})

#Need boost system and filesystem for crossplatform temp file creation.
find_package(Boost 1.36.0 COMPONENTS program_options filesystem system REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})

#Link our Logger against all the Cpps

set(CLIENT_SOURCE
	main.cpp 
	sigarsysteminfo.cpp
	logcontroller.cpp
)

set(CLIENT_HEADERS
	sigarsysteminfo.h
	logcontroller.h
)

add_executable(${PROJ_NAME} ${CLIENT_SOURCE} ${CLIENT_HEADERS} ${PROTO_SRCS} ${PROTO_HDRS})

#set_property(TARGET ${PROJ_NAME} PROPERTY CXX_INCLUDE_WHAT_YOU_USE ${iwyu_path})
if (MSVC)
	#Visual studio needs to be told to build in Multithreaded Static mode
        set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
else()
	#Unix needs to include pthread
	target_link_libraries(${PROJ_NAME} pthread dl)
endif()

target_link_libraries(${PROJ_NAME} zmqprotowriter)
target_link_libraries(${PROJ_NAME} re_common_proto_systemstatus)

#include the current binary directory to re_node_manager as it requires the .h file from the pb generated files
target_include_directories(${PROJ_NAME} PUBLIC ${CMAKE_CURRENT_BINARY_DIR})

#Link against our required libraries
target_link_libraries(${PROJ_NAME} ${PROTOBUF_LIBRARIES})
target_link_libraries(${PROJ_NAME} ${SIGAR_LIBRARIES})
target_link_libraries(${PROJ_NAME} ${Boost_PROGRAM_OPTIONS_LIBRARY})
target_link_libraries(${PROJ_NAME} ${Boost_FILESYSTEM_LIBRARY})
target_link_libraries(${PROJ_NAME} ${Boost_SYSTEM_LIBRARY})
