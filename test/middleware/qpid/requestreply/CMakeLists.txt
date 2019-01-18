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

target_include_directories(${PROJ_NAME} PRIVATE ${RE_SOURCE_PATH})
target_include_directories(${PROJ_NAME} PRIVATE ${RE_COMMON_SOURCE_DIR})
target_include_directories(${PROJ_NAME} PRIVATE ${QPID_INCLUDE_DIRS})
target_include_directories(${PROJ_NAME} PRIVATE ${CMAKE_CURRENT_BINARY_DIR})

if(MSVC)
    target_compile_definitions(${PROJ_NAME} PRIVATE -DPROTOBUF_USE_DLLS)
elseif(UNIX)
    target_link_libraries(${PROJ_NAME} PRIVATE pthread)
    target_link_libraries(${PROJ_NAME} PRIVATE dl)
endif()

target_link_libraries(${PROJ_NAME} PRIVATE re_core)
target_link_libraries(${PROJ_NAME} PRIVATE re_core_singletons)
target_link_libraries(${PROJ_NAME} PRIVATE gtest)
target_link_libraries(${PROJ_NAME} PRIVATE lib_test_basic_proto)
target_link_libraries(${PROJ_NAME} PRIVATE qpid_helper)
target_link_libraries(${PROJ_NAME} PRIVATE ${QPID_LIBRARIES})