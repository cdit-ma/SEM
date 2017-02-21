set(PROJ_NAME re_node_manager)
project(${PROJ_NAME})

#Find Protobuf
find_package(Protobuf REQUIRED)

#Find ZeroMQ
find_package(ZMQ REQUIRED)

find_package(Boost 1.36.0 COMPONENTS program_options REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})

#Find PUGIXML Library
set(PUGIXML_ROOT $ENV{PUGIXML_ROOT})
find_library(PUGIXML_LIBRARIES pugixml ${PUGIXML_ROOT}/build)

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

include_directories(${PUGIXML_ROOT}/src/)
include_directories(${PROJ_NAME} ${ZMQ_INCLUDE_DIRS})

add_executable(${PROJ_NAME} ${SOURCE} ${HEADERS})

target_include_directories(${PROJ_NAME} PUBLIC ${CMAKE_CURRENT_BINARY_DIR})

if(UNIX)
	target_link_libraries(${PROJ_NAME} pthread)
    target_link_libraries(${PROJ_NAME} dl)
endif(UNIX)

#Link executable against all libraries required
target_link_libraries(${PROJ_NAME} ${ZMQ_LIBRARIES})
target_link_libraries(${PROJ_NAME} ${PROTOBUF_LIBRARIES})
target_link_libraries(${PROJ_NAME} ${PUGIXML_LIBRARIES})
target_link_libraries(${PROJ_NAME} ${Boost_PROGRAM_OPTIONS_LIBRARY})
target_link_libraries(${PROJ_NAME} re_core)
target_link_libraries(${PROJ_NAME} re_proto_control)



