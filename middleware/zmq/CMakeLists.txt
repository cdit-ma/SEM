add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/message")


#Get the RE_PATH
set(RE_PATH "$ENV{RE_PATH}")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${RE_PATH}/cmake/modules")

set(PROJ_NAME zmq_ports)
project (${PROJ_NAME})

#Find re_core library
find_library(RE_CORE_LIBRARIES re_core "${RE_PATH}/lib")

set(SOURCE
	${CMAKE_CURRENT_SOURCE_DIR}/test.cpp
	)

set(HEADERS
	${CMAKE_CURRENT_SOURCE_DIR}/test.h
	)

include_directories("${RE_PATH}/includes")

#Set symbols in dynamic library to be default hidden
set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} " -fvisibility=hidden")

add_library(${PROJ_NAME} MODULE ${SOURCE} ${HEADERS})

target_link_libraries(${PROJ_NAME} ${RE_CORE_LIBRARIES})

target_link_libraries(${PROJ_NAME} zmq_message)