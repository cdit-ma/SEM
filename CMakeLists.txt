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


set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/lib")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/lib")

#Add all bits for Deployment
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/ports/message")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/ports/message/zmq")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/ports/message/proto")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/components/hellosender")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/components/helloreceiver")


set(SOURCE
	${CMAKE_CURRENT_SOURCE_DIR}/deployment_2.cpp
	)
set(HEADERS
	${CMAKE_CURRENT_SOURCE_DIR}/deployment_2.h
	)

message(${RE_PATH})
include_directories("${RE_PATH}/includes")

add_library(deployment_2 MODULE ${SOURCE} ${HEADERS})

if (MSVC)
	#Visual studio needs to be told to build in Multithreaded Static mode
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
endif()

find_library(RE_CORE_LIBRARIES re_core PATHS "${RE_PATH}/lib")
find_library(RE_PROTO_CONTROL_LIBRARIES re_proto_control PATHS "${RE_PATH}/lib")

target_link_libraries(deployment_2 ${RE_CORE_LIBRARIES})
target_link_libraries(deployment_2 ${RE_PROTO_CONTROL_LIBRARIES})
target_link_libraries(deployment_2 components_hellosender)
target_link_libraries(deployment_2 components_helloreceiver)
target_link_libraries(deployment_2 ports_message_zmq)