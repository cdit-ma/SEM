set(RE_NODE_MANAGER re_node_manager)
set(RE_NODE_MANAGER_LIB ${RE_NODE_MANAGER}_lib)
set(RE_EXECUTION_TESTER re_execution_tester)

set(RE_MASTER re_experiment_master)
set(RE_SLAVE re_experiment_slave)

set(PROJ_NAME ${RE_NODE_MANAGER})
project(${PROJ_NAME})

#Link against boost dynamically
find_package(Boost 1.30.0 COMPONENTS program_options REQUIRED)
find_package(Protobuf REQUIRED)
find_package(ZMQ REQUIRED)

#Build Supporting libraries
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/controlmessage")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/executionparser")

set(SOURCE
    ${CMAKE_CURRENT_SOURCE_DIR}/executionmanager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/experimentmaster.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/experimentslave.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/deploymentmanager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/deploymentcontainer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/dllloader.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/zmq/registrant.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/zmq/registrar.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/environmentrequester.cpp
    )

set(HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/executionmanager.h
    ${CMAKE_CURRENT_SOURCE_DIR}/deploymentmanager.h
    ${CMAKE_CURRENT_SOURCE_DIR}/deploymentcontainer.h
    ${CMAKE_CURRENT_SOURCE_DIR}/dllloader.h
    ${CMAKE_CURRENT_SOURCE_DIR}/zmq/registrant.h
    ${CMAKE_CURRENT_SOURCE_DIR}/zmq/registrar.h
    ${CMAKE_CURRENT_SOURCE_DIR}/environmentrequester.h
    )

#Pass Version info into cmakevars.h
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/cmakevars.h.in ${CMAKE_CURRENT_BINARY_DIR}/cmakevars.h)

add_library(${RE_NODE_MANAGER_LIB} STATIC ${SOURCE} ${HEADERS})

add_executable(${RE_MASTER} "${CMAKE_CURRENT_SOURCE_DIR}/experimentmaster.cpp")
add_executable(${RE_SLAVE} "${CMAKE_CURRENT_SOURCE_DIR}/experimentslave.cpp")
add_executable(${RE_EXECUTION_TESTER} "${CMAKE_CURRENT_SOURCE_DIR}/executiontester.cpp")
#add_executable(requester_tester "${CMAKE_CURRENT_SOURCE_DIR}/requester_tester.cpp")

# Remove the need for _lib extensions
set_target_properties(${RE_NODE_MANAGER_LIB} PROPERTIES OUTPUT_NAME ${RE_NODE_MANAGER})

target_include_directories(${RE_NODE_MANAGER_LIB} PRIVATE ${RE_SRC_PATH})
target_include_directories(${RE_NODE_MANAGER_LIB} PRIVATE ${PROTOBUF_INCLUDE_DIRS})
target_include_directories(${RE_NODE_MANAGER_LIB} PRIVATE ${ZMQ_INCLUDE_DIRS})
target_include_directories(${RE_NODE_MANAGER_LIB} PRIVATE ${CMAKE_CURRENT_BINARY_DIR})

# Include the RE_SRC
target_include_directories(${RE_MASTER} PRIVATE ${RE_SRC_PATH})
target_include_directories(${RE_MASTER} PRIVATE ${PROTOBUF_INCLUDE_DIRS})
target_include_directories(${RE_SLAVE} PRIVATE ${RE_SRC_PATH})
target_include_directories(${RE_SLAVE} PRIVATE ${PROTOBUF_INCLUDE_DIRS})
target_include_directories(${RE_EXECUTION_TESTER} PRIVATE ${PROTOBUF_INCLUDE_DIRS})
target_include_directories(${RE_EXECUTION_TESTER} PRIVATE ${RE_SRC_PATH})

target_include_directories(${RE_MASTER} PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
target_include_directories(${RE_MASTER} PRIVATE ${Boost_INCLUDE_DIRS})

target_include_directories(${RE_SLAVE} PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
target_include_directories(${RE_SLAVE} PRIVATE ${Boost_INCLUDE_DIRS})

target_include_directories(${RE_EXECUTION_TESTER} PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
target_include_directories(${RE_EXECUTION_TESTER} PRIVATE ${Boost_INCLUDE_DIRS})


if (MSVC)
    # Windows requires protobuf in DLLs
    target_compile_definitions(${RE_MASTER} PRIVATE -DBOOST_ALL_DYN_LINK)
    target_compile_definitions(${RE_SLAVE} PRIVATE -DBOOST_ALL_DYN_LINK)
    target_compile_definitions(${RE_EXECUTION_TESTER} PRIVATE -DBOOST_ALL_DYN_LINK)
    target_compile_definitions(${RE_NODE_MANAGER_LIB} PRIVATE -DPROTOBUF_USE_DLLS)
elseif(UNIX)
	target_link_libraries(${RE_NODE_MANAGER_LIB} pthread)
    target_link_libraries(${RE_NODE_MANAGER_LIB} dl)
endif(MSVC)

#Link against required libraries
target_link_libraries(${RE_NODE_MANAGER_LIB} ${ZMQ_LIBRARIES})
target_link_libraries(${RE_NODE_MANAGER_LIB} ${PROTOBUF_LIBRARIES})
target_link_libraries(${RE_NODE_MANAGER_LIB} re_core)
target_link_libraries(${RE_NODE_MANAGER_LIB} zmq_protoreceiver)
target_link_libraries(${RE_NODE_MANAGER_LIB} zmq_protowriter)
target_link_libraries(${RE_NODE_MANAGER_LIB} re_execution_parser_lib)
target_link_libraries(${RE_NODE_MANAGER_LIB} re_proto_control)
target_link_libraries(${RE_NODE_MANAGER_LIB} environment_manager_lib)

target_link_libraries(${RE_MASTER} ${Boost_PROGRAM_OPTIONS_LIBRARY})
target_link_libraries(${RE_SLAVE} ${Boost_PROGRAM_OPTIONS_LIBRARY})
target_link_libraries(${RE_EXECUTION_TESTER} ${Boost_PROGRAM_OPTIONS_LIBRARY})

target_link_libraries(${RE_MASTER} ${RE_NODE_MANAGER_LIB})
target_link_libraries(${RE_SLAVE} ${RE_NODE_MANAGER_LIB})
target_link_libraries(${RE_EXECUTION_TESTER} ${RE_NODE_MANAGER_LIB})
