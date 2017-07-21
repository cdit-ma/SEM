cmake_minimum_required (VERSION 2.6)

#Use C++11
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
#Set our module path
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "$ENV{CMAKE_MODULE_PATH}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/")


set(PROJ_NAME re_node_manager)
set(PROJ_NAME2 execution_tester)
project(${PROJ_NAME})

#Find Protobuf
find_package(Protobuf REQUIRED)

#Find ZeroMQ
find_package(ZMQ REQUIRED)

#Link against boost dynamically
add_definitions(-DBOOST_ALL_DYN_LINK)
find_package(Boost 1.30.0 COMPONENTS program_options REQUIRED)

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/executionparser")

set(SOURCE
    ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/executionmanager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/deploymentmanager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/nodecontainer.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/zmq/registrant.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/zmq/registrar.cpp
	)

set(HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/executionmanager.h
    ${CMAKE_CURRENT_SOURCE_DIR}/deploymentmanager.h
    ${CMAKE_CURRENT_SOURCE_DIR}/nodecontainer.h

    ${CMAKE_CURRENT_SOURCE_DIR}/zmq/registrant.h
    ${CMAKE_CURRENT_SOURCE_DIR}/zmq/registrar.h
    )
    
    set(SOURCE2
    ${CMAKE_CURRENT_SOURCE_DIR}/executiontester.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/executionmanager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/deploymentmanager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/nodecontainer.cpp
	)

set(HEADERS2
    ${CMAKE_CURRENT_SOURCE_DIR}/executionmanager.h
    ${CMAKE_CURRENT_SOURCE_DIR}/deploymentmanager.h
    ${CMAKE_CURRENT_SOURCE_DIR}/nodecontainer.h
	)

include_directories(${Boost_INCLUDE_DIRS})
include_directories(${ZMQ_INCLUDE_DIRS})
include_directories(${PROTOBUF_INCLUDE_DIRS})


add_executable(${PROJ_NAME} ${SOURCE} ${HEADERS})
add_executable(${PROJ_NAME2} ${SOURCE2} ${HEADERS})

target_include_directories(${PROJ_NAME} PUBLIC ${CMAKE_CURRENT_BINARY_DIR})
target_include_directories(${PROJ_NAME2} PUBLIC ${CMAKE_CURRENT_BINARY_DIR})

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
target_link_libraries(${PROJ_NAME} ${Boost_PROGRAM_OPTIONS_LIBRARY})
target_link_libraries(${PROJ_NAME} re_core)
target_link_libraries(${PROJ_NAME} zmq_protoreceiver)
target_link_libraries(${PROJ_NAME} zmq_protowriter)
target_link_libraries(${PROJ_NAME} re_execution_parser_lib)

target_link_libraries(${PROJ_NAME2} ${ZMQ_LIBRARIES})
target_link_libraries(${PROJ_NAME2} ${PROTOBUF_LIBRARIES})
target_link_libraries(${PROJ_NAME2} ${Boost_PROGRAM_OPTIONS_LIBRARY})
target_link_libraries(${PROJ_NAME2} re_core)
target_link_libraries(${PROJ_NAME2} zmq_protoreceiver)
target_link_libraries(${PROJ_NAME2} zmq_protowriter)
target_link_libraries(${PROJ_NAME2} re_execution_parser_lib)





