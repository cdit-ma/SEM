cmake_minimum_required (VERSION 2.6)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

#Set the projects RPATH to look in the executable directory
set(CMAKE_MACOSX_RPATH 1)
set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
set(CMAKE_INSTALL_RPATH "@executable_path;@loader_path;$ORIGIN;")

set(RE_PATH "$ENV{RE_PATH}")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${RE_PATH}/cmake/modules")


set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/../bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/../lib")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/../lib")

#Add all bits for Deployment
#add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/ports/message" ${CMAKE_CURRENT_BINARY_DIR})
#add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/ports/message/zmq" ${CMAKE_CURRENT_BINARY_DIR})
#add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/ports/message/proto" ${CMAKE_CURRENT_BINARY_DIR})
#add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/components/hellosender" ${CMAKE_CURRENT_BINARY_DIR})
#add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/components/helloreceiver" ${CMAKE_CURRENT_BINARY_DIR})

SET(PROJ_NAME deployment_2)
PROJECT(${PROJ_NAME})

set(SOURCE
	${CMAKE_CURRENT_SOURCE_DIR}/deployment_2.cpp
	)
set(HEADERS
	${CMAKE_CURRENT_SOURCE_DIR}/deployment_2.h
	)

message(${RE_PATH})
include_directories("${RE_PATH}/includes")

add_library(${PROJ_NAME} MODULE ${SOURCE} ${HEADERS})

if (MSVC)
	#Visual studio needs to be told to build in Multithreaded Static mode
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
endif()

find_library(RE_CORE_LIBRARIES re_core PATHS "${RE_PATH}/lib")
find_library(RE_PROTO_CONTROL_LIBRARIES re_proto_control PATHS "${RE_PATH}/lib")

target_link_libraries(${PROJ_NAME} ${RE_CORE_LIBRARIES})
target_link_libraries(${PROJ_NAME} ${RE_PROTO_CONTROL_LIBRARIES})
target_link_libraries(${PROJ_NAME} components_hellosender)
target_link_libraries(${PROJ_NAME} components_helloreceiver)
target_link_libraries(${PROJ_NAME} ports_message_zmq)