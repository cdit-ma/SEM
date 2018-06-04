#Set our project names
set(PROJ_NAME test_zmq_requestreply)
project(${PROJ_NAME})

# Find package ZMQ
find_package(ZMQ)

#Find protobuf
find_package(Protobuf REQUIRED)

if(NOT ZMQ_FOUND)
	message(STATUS "Cannot find ZMQ cannot build project ${PROJ_NAME}")
	return()
endif()

SET(SOURCE
    ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
)

add_executable(${PROJ_NAME} ${SOURCE})

target_include_directories(${PROJ_NAME} PRIVATE ${RE_SRC_PATH})
target_include_directories(${PROJ_NAME} PRIVATE ${ZMQ_INCLUDE_DIRS})
target_include_directories(${PROJ_NAME} PRIVATE ${PROTOBUF_INCLUDE_DIRS})
target_include_directories(${PROJ_NAME} PRIVATE ${CMAKE_CURRENT_BINARY_DIR})

if(MSVC)
    target_compile_definitions(${PROJ_NAME} PRIVATE -DPROTOBUF_USE_DLLS)
elseif(UNIX)
    target_link_libraries(${PROJ_NAME} pthread)
    target_link_libraries(${PROJ_NAME} dl)
endif()

target_link_libraries(${PROJ_NAME} ${ZMQ_LIBRARIES})

target_link_libraries(${PROJ_NAME} re_core)
target_link_libraries(${PROJ_NAME} gtest)
target_link_libraries(${PROJ_NAME} zmq_helper)
target_link_libraries(${PROJ_NAME} lib_test_basic_proto)
