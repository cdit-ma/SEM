cmake_minimum_required (VERSION 2.6)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#Set our module path
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "$ENV{CMAKE_MODULE_PATH}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/")

#Include the parent directory
include_directories("../")



#Pass Version info into cmakevars.h
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/cmakevars.h.in ${CMAKE_CURRENT_BINARY_DIR}/cmakevars.h)

set(NODE_MANAGER re_node_manager)
set(EXECUTION_TESTER re_execution_tester)
    
set(PROJ_NAME ${NODE_MANAGER})
project(${PROJ_NAME})
    
    
#Find Protobuf
find_package(Protobuf REQUIRED)

#Find ZeroMQ
find_package(ZMQ REQUIRED)

#Link against boost dynamically
find_package(Boost 1.30.0 COMPONENTS program_options REQUIRED)


add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/controlmessage")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/executionparser")



set(SOURCE
    ${CMAKE_CURRENT_SOURCE_DIR}/executionmanager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/deploymentmanager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/deploymentcontainer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/dllloader.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/zmq/registrant.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/zmq/registrar.cpp
	)

set(HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/executionmanager.h
    ${CMAKE_CURRENT_SOURCE_DIR}/deploymentmanager.h
    ${CMAKE_CURRENT_SOURCE_DIR}/deploymentcontainer.h
    ${CMAKE_CURRENT_SOURCE_DIR}/dllloader.h
    ${CMAKE_CURRENT_SOURCE_DIR}/zmq/registrant.h
    ${CMAKE_CURRENT_SOURCE_DIR}/zmq/registrar.h
    )
    
set(EXECUTION_TESTER_SOURCE
    ${CMAKE_CURRENT_SOURCE_DIR}/executiontester.cpp
    ${SOURCE}
	)

set(NODE_MANAGER_SOURCE
    ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
    ${SOURCE}
	)

add_executable(${NODE_MANAGER} ${NODE_MANAGER_SOURCE} ${HEADERS})
add_executable(${EXECUTION_TESTER} ${EXECUTION_TESTER_SOURCE} ${HEADERS})

target_compile_definitions(${NODE_MANAGER} PRIVATE -DBOOST_ALL_DYN_LINK)
target_compile_definitions(${EXECUTION_TESTER} PRIVATE -DBOOST_ALL_DYN_LINK)

target_include_directories(${NODE_MANAGER} PRIVATE ${RE_SRC_PATH})
target_include_directories(${NODE_MANAGER} PRIVATE ${Boost_INCLUDE_DIRS})
target_include_directories(${NODE_MANAGER} PRIVATE ${PROTOBUF_INCLUDE_DIRS})
target_include_directories(${NODE_MANAGER} PRIVATE ${ZMQ_INCLUDE_DIRS})
target_include_directories(${NODE_MANAGER} PRIVATE ${CMAKE_CURRENT_BINARY_DIR})

target_include_directories(${EXECUTION_TESTER} PRIVATE ${RE_SRC_PATH})
target_include_directories(${EXECUTION_TESTER} PRIVATE ${Boost_INCLUDE_DIRS})
target_include_directories(${EXECUTION_TESTER} PRIVATE ${PROTOBUF_INCLUDE_DIRS})
target_include_directories(${EXECUTION_TESTER} PRIVATE ${ZMQ_INCLUDE_DIRS})
target_include_directories(${EXECUTION_TESTER} PRIVATE ${CMAKE_CURRENT_BINARY_DIR})





if (MSVC)
    #Windows requires protobuf in DLLs
    target_compile_definitions(${NODE_MANAGER} PRIVATE -DPROTOBUF_USE_DLLS)
    target_compile_definitions(${EXECUTION_TESTER} PRIVATE -DPROTOBUF_USE_DLLS)
elseif(UNIX)
	target_link_libraries(${NODE_MANAGER} pthread)
    target_link_libraries(${NODE_MANAGER} dl)

    target_link_libraries(${EXECUTION_TESTER} pthread)
    target_link_libraries(${EXECUTION_TESTER} dl)
endif(MSVC)

#Link executable against all libraries required
target_link_libraries(${NODE_MANAGER} ${ZMQ_LIBRARIES})
target_link_libraries(${NODE_MANAGER} ${PROTOBUF_LIBRARIES})
target_link_libraries(${NODE_MANAGER} ${Boost_PROGRAM_OPTIONS_LIBRARY})
target_link_libraries(${NODE_MANAGER} re_core)
target_link_libraries(${NODE_MANAGER} zmq_protoreceiver)
target_link_libraries(${NODE_MANAGER} zmq_protowriter)
target_link_libraries(${NODE_MANAGER} re_execution_parser_lib)

target_link_libraries(${EXECUTION_TESTER} ${ZMQ_LIBRARIES})
target_link_libraries(${EXECUTION_TESTER} ${PROTOBUF_LIBRARIES})
target_link_libraries(${EXECUTION_TESTER} ${Boost_PROGRAM_OPTIONS_LIBRARY})
target_link_libraries(${EXECUTION_TESTER} re_core)
target_link_libraries(${EXECUTION_TESTER} zmq_protoreceiver)
target_link_libraries(${EXECUTION_TESTER} zmq_protowriter)
target_link_libraries(${EXECUTION_TESTER} re_execution_parser_lib)





