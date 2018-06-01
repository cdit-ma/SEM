cmake_minimum_required (VERSION 2.6)

#Use C++11
set(CMAKE_CXX_STANDARD 11)

set(PROJ_NAME tao_test)
set(SEND_NAME tao_client)
set(RECV_NAME tao_server)

project (${PROJ_NAME})

# Find package TAO
find_package(TAO)

if(NOT TAO_FOUND)
	message(STATUS "Cannot find TAO cannot build project ${PROJ_NAME}")
	return()
endif()

# Run the tao Compiler over the <idl> files
TAO_GENERATE_CPP(TAO_CLIENT_SOURCES TAO_CLIENT_HEADER ${CMAKE_CURRENT_SOURCE_DIR}/message_client.idl)

TAO_GENERATE_CPP(TAO_SERVER_SOURCES TAO_SERVER_HEADER ${CMAKE_CURRENT_SOURCE_DIR}/message_server.idl)

#Link our Logger against all the Cpps
set(SEND_SOURCE
    ${CMAKE_CURRENT_SOURCE_DIR}/client.cpp
)

set(RECV_SOURCE
    ${CMAKE_CURRENT_SOURCE_DIR}/server.cpp
)

add_executable(${SEND_NAME} ${SEND_SOURCE} ${TAO_CLIENT_SOURCES})
add_executable(${RECV_NAME} ${RECV_SOURCE} ${TAO_SERVER_SOURCES})

target_include_directories(${SEND_NAME} PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
target_include_directories(${RECV_NAME} PRIVATE ${CMAKE_CURRENT_BINARY_DIR})

target_include_directories(${SEND_NAME} PRIVATE ${TAO_INCLUDE_DIRS})
target_include_directories(${RECV_NAME} PRIVATE ${TAO_INCLUDE_DIRS})

target_include_directories(${SEND_NAME} PRIVATE ${RE_SRC_PATH})
target_include_directories(${RECV_NAME} PRIVATE ${RE_SRC_PATH})

target_link_libraries(${SEND_NAME} pthread dl)
target_link_libraries(${SEND_NAME} ${TAO_LIBRARIES})


target_link_libraries(${RECV_NAME} pthread dl)
target_link_libraries(${RECV_NAME} ${TAO_LIBRARIES})

target_link_libraries(${SEND_NAME} tao_helper)
target_link_libraries(${RECV_NAME} tao_helper)