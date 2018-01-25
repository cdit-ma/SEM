set(PROJ_NAME "zmq_message")
project(${PROJ_NAME})

# Find package ZMQ
find_package(ZMQ)

if(NOT ZMQ_FOUND)
	message(STATUS "Cannot find ZMQ cannot build ${PROJ_NAME}")
	return()
endif()

# Find library re_core
find_library(RE_CORE_LIBRARIES re_core "${RE_PATH}/lib")

# Find library zmq_helper
find_library(ZMQ_HELPER_LIBRARIES zmq_helper "${RE_PATH}/lib")

add_library(${PROJ_NAME} MODULE ${CMAKE_CURRENT_SOURCE_DIR}/libportexport.cpp)
target_include_directories(${PROJ_NAME} PRIVATE "${RE_PATH}/src")
target_include_directories(${PROJ_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../..")
target_include_directories(${PROJ_NAME} PRIVATE "${ZMQ_INCLUDE_DIRS}")
# Include the current binary directory to allow inclusion of generated files
target_include_directories(${PROJ_NAME} PRIVATE "${CMAKE_CURRENT_BINARY_DIR}")
target_link_libraries(${PROJ_NAME} "${ZMQ_LIBRARIES}")
target_link_libraries(${PROJ_NAME} "${RE_CORE_LIBRARIES}")
target_link_libraries(${PROJ_NAME} "${ZMQ_HELPER_LIBRARIES}")
target_link_libraries(${PROJ_NAME} "proto_message_lib")

