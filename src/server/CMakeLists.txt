cmake_minimum_required (VERSION 2.6)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(PROJ_NAME logan_server)
project (${PROJ_NAME})

#Pass Version info into cmakevars.h
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/../cmakevars.h.in ${CMAKE_CURRENT_BINARY_DIR}/cmakevars.h)



find_package(Protobuf REQUIRED)
find_package(ZMQ REQUIRED)
find_package(Boost 1.30.0 COMPONENTS program_options REQUIRED)

set(SOURCE
    ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/server.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/sqlitedatabase.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/table.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/tableinsert.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/sqlite3.c
)

set(HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/protohandler.h
    ${CMAKE_CURRENT_SOURCE_DIR}/sqlitedatabase.h
    ${CMAKE_CURRENT_SOURCE_DIR}/table.h
    ${CMAKE_CURRENT_SOURCE_DIR}/tableinsert.h
    ${CMAKE_CURRENT_SOURCE_DIR}/sqlite3.h
)

#Link our Server against all the Cpps
add_executable(${PROJ_NAME} ${SOURCE} ${HEADERS})

#Link against boost dynamically
target_compile_definitions(${PROJ_NAME} PRIVATE -DBOOST_ALL_DYN_LINK)

if (MSVC)
else()
	#Unix needs to include pthread
	target_link_libraries(${PROJ_NAME} pthread)
	target_link_libraries(${PROJ_NAME} dl)
endif()


#include the current binary directory to re_node_manager as it requires the .h file from the pb generated files
target_include_directories(${PROJ_NAME} PRIVATE ${Boost_INCLUDE_DIRS})
target_include_directories(${PROJ_NAME} PRIVATE ${LOGAN_SRC_PATH})
target_include_directories(${PROJ_NAME} PRIVATE ${PROTOBUF_INCLUDE_DIRS})
target_include_directories(${PROJ_NAME} PRIVATE ${ZMQ_INCLUDE_DIRS})
target_include_directories(${PROJ_NAME} PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
target_include_directories(${PROJ_NAME} PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/../)
target_include_directories(${PROJ_NAME} PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/../re_common/proto/controlmessage)

target_link_libraries(${PROJ_NAME} zmq_protoreceiver)
target_link_libraries(${PROJ_NAME} re_common_proto_systemstatus)
target_link_libraries(${PROJ_NAME} re_common_proto_control)
target_link_libraries(${PROJ_NAME} zmq_environmentrequester)

target_link_libraries(${PROJ_NAME} ${Boost_PROGRAM_OPTIONS_LIBRARY})

if(NOT DISABLE_MODEL_LOGGING)
    add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/modelprotohandler")
    target_link_libraries(${PROJ_NAME} modelprotohandler)
else()
    target_compile_definitions(${PROJ_NAME} PRIVATE DISABLE_MODEL_HANDLER)
endif()

if(NOT DISABLE_HARDWARE_LOGGING)
    add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/hardwareprotohandler")
    target_link_libraries(${PROJ_NAME} hardwareprotohandler)
else()
    target_compile_definitions(${PROJ_NAME} PRIVATE DISABLE_HARDWARE_HANDLER)
endif()

