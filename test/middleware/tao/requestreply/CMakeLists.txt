#Set our project names
set(PROJ_NAME test_tao_requestreply)
set(CLIENT_NAME test_tao_client)
set(SERVER_NAME test_tao_server)
project(${PROJ_NAME})

# Find package ZMQ
find_package(TAO)


if(NOT TAO_FOUND)
	message(STATUS "Cannot find TAO cannot build project ${PROJ_NAME}")
	return()
endif()

SET(SOURCE
    ${CMAKE_CURRENT_SOURCE_DIR}/tests.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/translator.cpp
    ${RE_TEST_SOURCE_DIR}/core/test_main.cpp
)

SET(CLIENT_SOURCE
    ${CMAKE_CURRENT_SOURCE_DIR}/client.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/translator.cpp
)

SET(SERVER_SOURCE
    ${CMAKE_CURRENT_SOURCE_DIR}/server.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/translator.cpp
)
TAO_GENERATE_CPP(TAO_SRCS TAO_HDRS ${CMAKE_CURRENT_SOURCE_DIR}/../message.idl)

add_executable(${PROJ_NAME} ${SOURCE} ${TAO_SRCS} ${TAO_HDRS})
add_executable(${CLIENT_NAME} ${CLIENT_SOURCE} ${TAO_SRCS} ${TAO_HDRS})
add_executable(${SERVER_NAME} ${SERVER_SOURCE} ${TAO_SRCS} ${TAO_HDRS})

target_include_directories(${PROJ_NAME} PRIVATE ${RE_SRC_PATH})
target_include_directories(${PROJ_NAME} PRIVATE ${TAO_INCLUDE_DIRS})
target_include_directories(${PROJ_NAME} PRIVATE ${PROTOBUF_INCLUDE_DIRS})
target_include_directories(${PROJ_NAME} PRIVATE ${CMAKE_CURRENT_BINARY_DIR})

target_include_directories(${CLIENT_NAME} PRIVATE ${RE_SRC_PATH})
target_include_directories(${CLIENT_NAME} PRIVATE ${TAO_INCLUDE_DIRS})
target_include_directories(${CLIENT_NAME} PRIVATE ${PROTOBUF_INCLUDE_DIRS})
target_include_directories(${CLIENT_NAME} PRIVATE ${CMAKE_CURRENT_BINARY_DIR})

target_include_directories(${SERVER_NAME} PRIVATE ${RE_SRC_PATH})
target_include_directories(${SERVER_NAME} PRIVATE ${TAO_INCLUDE_DIRS})
target_include_directories(${SERVER_NAME} PRIVATE ${PROTOBUF_INCLUDE_DIRS})
target_include_directories(${SERVER_NAME} PRIVATE ${CMAKE_CURRENT_BINARY_DIR})

if(MSVC)

elseif(UNIX)
    target_link_libraries(${PROJ_NAME} pthread)
    target_link_libraries(${PROJ_NAME} dl)
endif()

target_link_libraries(${PROJ_NAME} ${TAO_LIBRARIES})

target_link_libraries(${PROJ_NAME} re_core)
target_link_libraries(${PROJ_NAME} gtest)
target_link_libraries(${PROJ_NAME} tao_helper)

target_link_libraries(${CLIENT_NAME} ${TAO_LIBRARIES})
target_link_libraries(${CLIENT_NAME} re_core)
target_link_libraries(${CLIENT_NAME} gtest)
target_link_libraries(${CLIENT_NAME} tao_helper)

target_link_libraries(${SERVER_NAME} ${TAO_LIBRARIES})
target_link_libraries(${SERVER_NAME} re_core)
target_link_libraries(${SERVER_NAME} gtest)
target_link_libraries(${SERVER_NAME} tao_helper)