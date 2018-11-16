#Set our project names
set(PROJ_NAME test_zmq_pubsub)
project(${PROJ_NAME})

# Find package ZMQ
find_package(ZMQ)

if(NOT ZMQ_FOUND)
	message(STATUS "Cannot find ZMQ cannot build project ${PROJ_NAME}")
	return()
endif()

SET(SOURCE
    ${CMAKE_CURRENT_SOURCE_DIR}/tests.cpp
    ${RE_TEST_SOURCE_DIR}/core/test_main.cpp
)

add_executable(${PROJ_NAME} ${SOURCE})

target_include_directories(${PROJ_NAME} PRIVATE ${RE_SOURCE_PATH})
target_include_directories(${PROJ_NAME} PRIVATE ${RE_COMMON_SOURCE_DIR})
target_include_directories(${PROJ_NAME} PRIVATE ${ZMQ_INCLUDE_DIRS})
target_include_directories(${PROJ_NAME} PRIVATE ${CMAKE_CURRENT_BINARY_DIR})

if(UNIX)
    target_link_libraries(${PROJ_NAME} PRIVATE pthread)
endif()

target_link_libraries(${PROJ_NAME} PRIVATE ${ZMQ_LIBRARIES})

target_link_libraries(${PROJ_NAME} PRIVATE re_core)
target_link_libraries(${PROJ_NAME} PRIVATE gtest)
target_link_libraries(${PROJ_NAME} PRIVATE zmq_helper)
target_link_libraries(${PROJ_NAME} PRIVATE lib_test_basic_proto)
target_link_libraries(${PROJ_NAME} PRIVATE re_core_singletons)
