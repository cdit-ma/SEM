cmake_minimum_required (VERSION 2.6)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/../bin")

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(PROJ_NAME re_node_manager)
project (${PROJ_NAME})

set(PUGIXML_ROOT "/home/dig/Downloads/pugixml-1.8/")


set(CDITMA_RUNTIME_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/../")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CDITMA_RUNTIME_ROOT}/cmake/modules")
include_directories(${PROJ_NAME} "${CDITMA_RUNTIME_ROOT}/includes")

#Find our core library
find_library(RE_CORE_LIBRARIES re_core "${CDITMA_RUNTIME_ROOT}/lib")

find_package(ZeroMQ REQUIRED)
include_directories(${PROJ_NAME} ${ZeroMQ_INCLUDE_DIRS})

find_package(Protobuf REQUIRED)
include_directories(${PROJ_NAME} ${PROTOBUF_INCLUDE_DIRS})

find_package(Boost 1.36.0)
include_directories(${Boost_INCLUDE_DIRS})

find_library(PUGIXML_LIBRARIES pugixml ${PUGIXML_ROOT}/build)
include_directories(${PUGIXML_ROOT}/src/)

protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS ${CMAKE_CURRENT_SOURCE_DIR}/controlmessage.proto)

set(SOURCE
	${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/deploymentmanager.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/zmqmaster.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/zmqslave.cpp
	${PROTO_SRCS}
	)

add_executable(${PROJ_NAME} ${SOURCE} ${PROTO_HDRS})

set(TESTSOURCE ${CMAKE_CURRENT_SOURCE_DIR}/parsemain.cpp
				${CMAKE_CURRENT_SOURCE_DIR}/graphmlparser.cpp)

add_executable("parsetest" ${TESTSOURCE})
target_link_libraries(parsetest ${PUGIXML_LIBRARIES})

if (MSVC)
	#Visual studio needs to be told to build in Multithreaded Static mode
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
endif()

if(UNIX)
	target_link_libraries(${PROJ_NAME} dl)
endif(UNIX)


target_link_libraries(${PROJ_NAME} ${RE_CORE_LIBRARIES})
target_link_libraries(${PROJ_NAME} ${ZeroMQ_LIBRARIES})
target_link_libraries(${PROJ_NAME} ${PROTOBUF_LIBRARIES})
target_link_libraries(${PROJ_NAME} ${Boost_PROPERTY_TREE_LIBRARY})

target_link_libraries(${PROJ_NAME} pthread)
target_link_libraries(${PROJ_NAME} ${PUGIXML_LIBRARIES})
target_include_directories(${PROJ_NAME} PUBLIC ${CMAKE_CURRENT_BINARY_DIR})

target_link_libraries(${PROJ_NAME} pthread)
