#Set our project names
set(PROJ_NAME tao_requester)
project(${PROJ_NAME})

# Find package ZMQ
find_package(TAO)

if(NOT TAO_FOUND)
	message(STATUS "Cannot find TAO cannot build project ${PROJ_NAME}")
	return()
endif()

SET(SOURCE
    ${CMAKE_CURRENT_SOURCE_DIR}/requester.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/translator.cpp
)

TAO_GENERATE_CPP(TAO_SRCS TAO_HDRS ${CMAKE_CURRENT_SOURCE_DIR}/message.idl)

add_executable(${PROJ_NAME} ${SOURCE} ${TAO_SRCS} ${TAO_HDRS})

target_include_directories(${PROJ_NAME} PRIVATE ${RE_SRC_PATH})
target_include_directories(${PROJ_NAME} PRIVATE ${TAO_INCLUDE_DIRS})
target_include_directories(${PROJ_NAME} PRIVATE ${PROTOBUF_INCLUDE_DIRS})
target_include_directories(${PROJ_NAME} PRIVATE ${CMAKE_CURRENT_BINARY_DIR})

if(UNIX)
    target_link_libraries(${PROJ_NAME} pthread)
    target_link_libraries(${PROJ_NAME} dl)
endif()

target_link_libraries(${PROJ_NAME} ${TAO_LIBRARIES})

target_link_libraries(${PROJ_NAME} re_core)
target_link_libraries(${PROJ_NAME} gtest)
target_link_libraries(${PROJ_NAME} tao_helper)