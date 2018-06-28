#Set our project names
set(PROJ_NAME test_tao_requestreply)
project(${PROJ_NAME})

# Find package ZMQ
find_package(TAO)


if(NOT TAO_FOUND)
	message(STATUS "Cannot find TAO cannot build project ${PROJ_NAME}")
	return()
endif()

SET(SOURCE
    ${CMAKE_CURRENT_SOURCE_DIR}/tests.cpp
    ${GOOGLE_TEST_DIR}/src/gtest_main.cc
)

TAO_GENERATE_CPP(TAO_SRCS TAO_HDRS ${CMAKE_CURRENT_SOURCE_DIR}/../basic.idl)

add_executable(${PROJ_NAME} ${SOURCE} ${TAO_SRCS} ${TAO_HDRS})

target_include_directories(${PROJ_NAME} PRIVATE ${RE_SRC_PATH})
target_include_directories(${PROJ_NAME} PRIVATE ${TAO_INCLUDE_DIRS})
target_include_directories(${PROJ_NAME} PRIVATE ${PROTOBUF_INCLUDE_DIRS})
target_include_directories(${PROJ_NAME} PRIVATE ${CMAKE_CURRENT_BINARY_DIR})

if(MSVC)

elseif(UNIX)
    target_link_libraries(${PROJ_NAME} pthread)
    target_link_libraries(${PROJ_NAME} dl)
endif()

target_link_libraries(${PROJ_NAME} ${TAO_LIBRARIES})

target_link_libraries(${PROJ_NAME} re_core)
target_link_libraries(${PROJ_NAME} gtest)
target_link_libraries(${PROJ_NAME} tao_helper)
target_link_libraries(${PROJ_NAME} lib_test_basic_proto)
