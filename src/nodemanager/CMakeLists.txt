set(RE_NODE_MANAGER re_node_manager)
set(RE_NODE_MANAGER_LIB ${RE_NODE_MANAGER}_lib)
set(RE_EXECUTION_TESTER re_execution_tester)

set(PROJ_NAME ${RE_NODE_MANAGER})
project(${PROJ_NAME})

#Link against boost dynamically
find_package(Boost 1.30.0 COMPONENTS program_options REQUIRED)
find_package(Protobuf REQUIRED)
find_package(ZMQ REQUIRED)

set(SOURCE
    ${CMAKE_CURRENT_SOURCE_DIR}/executionmanager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/deploymentmanager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/deploymentcontainer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/loganclient.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/dllloader.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/zmq/registrant.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/zmq/registrar.cpp
    )

set(HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/executionmanager.h
    ${CMAKE_CURRENT_SOURCE_DIR}/deploymentmanager.h
    ${CMAKE_CURRENT_SOURCE_DIR}/deploymentcontainer.h
    ${CMAKE_CURRENT_SOURCE_DIR}/loganclient.h
    ${CMAKE_CURRENT_SOURCE_DIR}/dllloader.h
    ${CMAKE_CURRENT_SOURCE_DIR}/zmq/registrant.h
    ${CMAKE_CURRENT_SOURCE_DIR}/zmq/registrar.h
    )

#Pass Version info into cmakevars.h
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/../cmakevars.h.in ${CMAKE_CURRENT_BINARY_DIR}/cmakevars.h)

add_library(${RE_NODE_MANAGER_LIB} SHARED ${SOURCE} ${HEADERS})

add_executable(${RE_NODE_MANAGER} "${CMAKE_CURRENT_SOURCE_DIR}/nodemanager.cpp")
add_executable(${RE_EXECUTION_TESTER} "${CMAKE_CURRENT_SOURCE_DIR}/executiontester.cpp")
#add_executable(requester_tester "${CMAKE_CURRENT_SOURCE_DIR}/requester_tester.cpp")

# Remove the need for _lib extensions
set_target_properties(${RE_NODE_MANAGER_LIB} PROPERTIES OUTPUT_NAME ${RE_NODE_MANAGER})

target_include_directories(${RE_NODE_MANAGER_LIB} PRIVATE ${PROTOBUF_INCLUDE_DIRS})
target_include_directories(${RE_NODE_MANAGER_LIB} PRIVATE ${ZMQ_INCLUDE_DIRS})
target_include_directories(${RE_NODE_MANAGER_LIB} PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
target_include_directories(${RE_NODE_MANAGER_LIB} PRIVATE ${RE_COMMON_BINARY_DIR})
target_include_directories(${RE_NODE_MANAGER_LIB} PRIVATE ${RE_COMMON_SOURCE_DIR})
target_include_directories(${RE_NODE_MANAGER_LIB} PRIVATE ${RE_SRC_PATH})
target_include_directories(${RE_NODE_MANAGER_LIB} PRIVATE ${LOGAN_SOURCE_DIR})

target_include_directories(${RE_NODE_MANAGER} PRIVATE ${PROTOBUF_INCLUDE_DIRS})
target_include_directories(${RE_NODE_MANAGER} PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
target_include_directories(${RE_NODE_MANAGER} PRIVATE ${Boost_INCLUDE_DIRS})
target_include_directories(${RE_NODE_MANAGER} PRIVATE ${RE_COMMON_SOURCE_DIR})
target_include_directories(${RE_NODE_MANAGER} PRIVATE ${RE_COMMON_BINARY_DIR})

target_include_directories(${RE_NODE_MANAGER} PRIVATE ${RE_SRC_PATH})

target_include_directories(${RE_EXECUTION_TESTER} PRIVATE ${PROTOBUF_INCLUDE_DIRS})
target_include_directories(${RE_EXECUTION_TESTER} PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
target_include_directories(${RE_EXECUTION_TESTER} PRIVATE ${Boost_INCLUDE_DIRS})
target_include_directories(${RE_EXECUTION_TESTER} PRIVATE ${RE_COMMON_SOURCE_DIR})
target_include_directories(${RE_EXECUTION_TESTER} PRIVATE ${RE_COMMON_BINARY_DIR})
target_include_directories(${RE_EXECUTION_TESTER} PRIVATE ${RE_SRC_PATH})



if (MSVC)
    # Windows requires protobuf in DLLs
    target_compile_definitions(${RE_NODE_MANAGER} PRIVATE -DBOOST_ALL_DYN_LINK)
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
target_link_libraries(${RE_NODE_MANAGER_LIB} zmq_environmentrequester)
target_link_libraries(${RE_NODE_MANAGER_LIB} re_execution_parser_lib)
target_link_libraries(${RE_NODE_MANAGER_LIB} re_common_proto_control)
target_link_libraries(${RE_NODE_MANAGER_LIB} graphml_parser_lib)
target_link_libraries(${RE_NODE_MANAGER_LIB} re_environment_manager_lib)
target_link_libraries(${RE_NODE_MANAGER_LIB} logan_client_lib)

target_link_libraries(${RE_NODE_MANAGER} ${Boost_PROGRAM_OPTIONS_LIBRARY})
target_link_libraries(${RE_EXECUTION_TESTER} ${Boost_PROGRAM_OPTIONS_LIBRARY})

target_link_libraries(${RE_NODE_MANAGER} ${RE_NODE_MANAGER_LIB})
target_link_libraries(${RE_EXECUTION_TESTER} ${RE_NODE_MANAGER_LIB})
