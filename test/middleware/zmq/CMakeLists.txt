#Set our project names
set(PROJ_NAME test_zmq)
project(${PROJ_NAME})

#Find protobuf
find_package(Protobuf REQUIRED)

# Find package ZMQ
find_package(ZMQ)

if(NOT ZMQ_FOUND)
	message(STATUS "Cannot find ZMQ cannot build project ${PROJ_NAME}")
	return()
endif()

#Generate out protobuf for control
protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS ${CMAKE_CURRENT_SOURCE_DIR}/basic.proto)

SET(SOURCE
    ${CMAKE_CURRENT_SOURCE_DIR}/zmqtest.cpp
)
add_executable(${PROJ_NAME} ${SOURCE} ${PROTO_SRCS} ${PROTO_HDRS})

target_include_directories(${PROJ_NAME} PRIVATE ${RE_SRC_PATH})
target_include_directories(${PROJ_NAME} PRIVATE ${PROTOBUF_INCLUDE_DIRS})
target_include_directories(${PROJ_NAME} PRIVATE ${ZMQ_INCLUDE_DIRS})
target_include_directories(${PROJ_NAME} PRIVATE ${CMAKE_CURRENT_BINARY_DIR})

if(MSVC)
    target_compile_definitions(${PROJ_NAME} PRIVATE -DPROTOBUF_USE_DLLS)
elseif(UNIX)
    target_link_libraries(${PROJ_NAME} pthread)
    target_link_libraries(${PROJ_NAME} dl)
endif()

target_link_libraries(${PROJ_NAME} re_core)
target_link_libraries(${PROJ_NAME} gtest)
target_link_libraries(${PROJ_NAME} zmq_helper)
