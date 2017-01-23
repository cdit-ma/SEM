cmake_minimum_required (VERSION 2.6)

#set(CMAKE_INSTALL_RPATH "$ORIGIN")
#set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE) # only if you want copy from the build tree

#SET( CMAKE_SHARED_LINKER_FLAGS  "-rpath='$ORIGIN/../lib'" )

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/../../bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/../../lib")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/../../lib")

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(PROJ_NAME re_core)
set(PROJ2_NAME re_proto_control)
set(PROJ3_NAME re_node_manager)
project (${PROJ_NAME})


if(APPLE)
    set(PUGIXML_ROOT "/Users/dan/pugixml-1.8/")
else(APPLE)
    set(PUGIXML_ROOT "/home/dig/Downloads/pugixml-1.8/")
endif(APPLE)

set(CDITMA_RUNTIME_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/../../../common")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CDITMA_RUNTIME_ROOT}/cmake/modules")
include_directories(${PROJ_NAME} "${CDITMA_RUNTIME_ROOT}/includes")

find_package(Protobuf REQUIRED)
include_directories(${PROJ2_NAME} ${PROTOBUF_INCLUDE_DIRS})
protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS ${CMAKE_CURRENT_SOURCE_DIR}/controlmessage.proto)

find_package(ZeroMQ REQUIRED)
include_directories(${PROJ_NAME} ${ZeroMQ_INCLUDE_DIRS})


find_library(PUGIXML_LIBRARIES pugixml ${PUGIXML_ROOT}/build)
include_directories(${PUGIXML_ROOT}/src/)

 
set(CORE_SRC
    ${CMAKE_CURRENT_SOURCE_DIR}/activatable.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/component.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/nodecontainer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/periodiceventport.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/basemessage.cpp
	)

set(CORE_HDRS
    ${CMAKE_CURRENT_SOURCE_DIR}/activatable.h
    ${CMAKE_CURRENT_SOURCE_DIR}/component.h
    ${CMAKE_CURRENT_SOURCE_DIR}/eventport.h
    ${CMAKE_CURRENT_SOURCE_DIR}/nodecontainer.h
    ${CMAKE_CURRENT_SOURCE_DIR}/periodiceventport.h
    ${CMAKE_CURRENT_SOURCE_DIR}/basemessage.h
	)

set(NMGR_SRC
    ${CMAKE_CURRENT_SOURCE_DIR}/nodemanager/deploymentmanager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/nodemanager/zmqmaster.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/nodemanager/zmqslave.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/nodemanager/main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/nodemanager/graphmlparser.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/nodemanager/executionmanager.cpp
	)

if (MSVC)
	#Visual studio needs to be told to build in Multithreaded Static mode
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
endif()



set(SOURCE2
    ${CMAKE_CURRENT_SOURCE_DIR}/translate.cpp
	)

add_library(${PROJ_NAME} SHARED ${CORE_SRC} ${CORE_HDRS})
add_library(${PROJ2_NAME} SHARED ${SOURCE2} ${PROTO_SRCS} ${PROTO_HDRS})
add_executable(${PROJ3_NAME} ${NMGR_SRC})


target_include_directories(${PROJ_NAME} PUBLIC ${CMAKE_CURRENT_BINARY_DIR})
target_include_directories(${PROJ2_NAME} PUBLIC ${CMAKE_CURRENT_BINARY_DIR})
target_include_directories(${PROJ3_NAME} PUBLIC ${CMAKE_CURRENT_BINARY_DIR})

target_link_libraries(${PROJ_NAME} ${PROJ2_NAME})

target_link_libraries(${PROJ2_NAME} ${PROTOBUF_LIBRARIES})

if(UNIX)
	target_link_libraries(${PROJ_NAME} pthread)

    target_link_libraries(${PROJ3_NAME} pthread)
    target_link_libraries(${PROJ3_NAME} dl)
endif(UNIX)

set(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} /home/dig/re/common/lib)

find_library(RE_CORE_LIBRARIES re_core PATHS /home/dig/re/common/lib)
find_library(RE_PROTO_CONTROL_LIBRARIES re_proto_control PATHS /home/dig/re/common/lib)
message(${RE_CORE_LIBRARIES})


#target_link_libraries(${PROJ3_NAME} ${PROJ2_NAME})
#target_link_libraries(${PROJ3_NAME} ${PROJ_NAME})
target_link_libraries(${PROJ3_NAME} ${ZeroMQ_LIBRARIES})
target_link_libraries(${PROJ3_NAME} ${PROTOBUF_LIBRARIES})
target_link_libraries(${PROJ3_NAME} ${PUGIXML_LIBRARIES})
target_link_libraries(${PROJ3_NAME} ${RE_CORE_LIBRARIES})
target_link_libraries(${PROJ3_NAME} ${RE_PROTO_CONTROL_LIBRARIES})

