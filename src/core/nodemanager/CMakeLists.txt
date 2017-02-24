cmake_minimum_required (VERSION 2.6)

#Use C++11
set(CMAKE_CXX_STANDARD 11)
#Set our module path
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules/")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/")


set(PROJ_NAME re_node_manager)
project(${PROJ_NAME})

#Find Protobuf
find_package(Protobuf REQUIRED)

#Find ZeroMQ
find_package(ZMQ REQUIRED)

#Link against boost dynamically
add_definitions(-DBOOST_ALL_DYN_LINK)
find_package(Boost 1.30.0 COMPONENTS program_options REQUIRED)


#Find PUGIXML Library
set(PUGIXML_ROOT $ENV{PUGIXML_ROOT})
find_library(PUGIXML_LIBRARIES pugixml ${PUGIXML_ROOT}/lib)

set(SOURCE
    ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/zmqslave.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/zmqmaster.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/graphmlparser.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/executionmanager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/deploymentmanager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/nodecontainer.cpp

	)

set(HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/zmqslave.h
    ${CMAKE_CURRENT_SOURCE_DIR}/zmqmaster.h
    ${CMAKE_CURRENT_SOURCE_DIR}/graphmlparser.h
    ${CMAKE_CURRENT_SOURCE_DIR}/executionmanager.h
    ${CMAKE_CURRENT_SOURCE_DIR}/deploymentmanager.h
    ${CMAKE_CURRENT_SOURCE_DIR}/nodecontainer.h
	)

include_directories(${Boost_INCLUDE_DIRS})
include_directories(${PUGIXML_ROOT}/include)
include_directories(${ZMQ_INCLUDE_DIRS})
include_directories(${PROTOBUF_INCLUDE_DIRS})



add_executable(${PROJ_NAME} ${SOURCE} ${HEADERS})

target_include_directories(${PROJ_NAME} PUBLIC ${CMAKE_CURRENT_BINARY_DIR})

message(${PROJ_NAME} PUBLIC ${CMAKE_CURRENT_BINARY_DIR})


if (MSVC)
	#Windows requires protobuf in DLLs
    add_definitions(-DPROTOBUF_USE_DLLS)
elseif(UNIX)
	target_link_libraries(${PROJ_NAME} pthread)
    target_link_libraries(${PROJ_NAME} dl)
endif(MSVC)

#target_link_libraries(${PROJ_NAME} re_proto_control)

#Link executable against all libraries required
target_link_libraries(${PROJ_NAME} ${ZMQ_LIBRARIES})
target_link_libraries(${PROJ_NAME} ${PROTOBUF_LIBRARIES})
target_link_libraries(${PROJ_NAME} ${PUGIXML_LIBRARIES})
target_link_libraries(${PROJ_NAME} ${Boost_PROGRAM_OPTIONS_LIBRARY})
target_link_libraries(${PROJ_NAME} re_core)




