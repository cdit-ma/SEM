#Set our project names
set(PROJ_NAME test_qpid_requestreply)
project(${PROJ_NAME})

#Find protobuf
find_package(Protobuf REQUIRED)

# Find package QPID
find_package(QPID)

if(NOT QPID_FOUND)
	message(STATUS "Cannot find QPID cannot build project ${PROJ_NAME}")
	return()
endif()

SET(SOURCE
    ${CMAKE_CURRENT_SOURCE_DIR}/tests.cpp
    ${RE_TEST_SOURCE_DIR}/core/test_main.cpp
)

add_executable(${PROJ_NAME} ${SOURCE} ${PROTO_SRCS} ${PROTO_HDRS})

target_include_directories(${PROJ_NAME} PRIVATE ${RE_SRC_PATH})
target_include_directories(${PROJ_NAME} PRIVATE ${RE_COMMON_SOURCE_PATH})
target_include_directories(${PROJ_NAME} PRIVATE ${QPID_INCLUDE_DIRS})
target_include_directories(${PROJ_NAME} PRIVATE ${PROTOBUF_INCLUDE_DIRS})
target_include_directories(${PROJ_NAME} PRIVATE ${CMAKE_CURRENT_BINARY_DIR})

if(MSVC)
    target_compile_definitions(${PROJ_NAME} PRIVATE -DPROTOBUF_USE_DLLS)
elseif(UNIX)
    target_link_libraries(${PROJ_NAME} pthread)
    target_link_libraries(${PROJ_NAME} dl)
endif()

target_link_libraries(${PROJ_NAME} re_core)
target_link_libraries(${PROJ_NAME} gtest)
target_link_libraries(${PROJ_NAME} lib_test_basic_proto)
target_link_libraries(${PROJ_NAME} qpid_helper)
target_link_libraries(${PROJ_NAME} ${QPID_LIBRARIES})