set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

#Get the RE_PATH
set(RE_PATH "$ENV{RE_PATH}")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${RE_PATH}/cmake/modules")

#Find ZeroMQ
find_package(ZeroMQ REQUIRED)

set(PROJ_NAME zmq_helper)
project (${PROJ_NAME})

set(SOURCE
    ${CMAKE_CURRENT_SOURCE_DIR}/zmqhelper.cpp
	)

set(HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/zmqhelper.h
    )

add_library(${PROJ_NAME} SHARED ${SOURCE} ${HEADERS})

include_directories("${RE_PATH}/includes")
include_directories(${ZeroMQ_INCLUDE_DIRS})

target_link_libraries(${PROJ_NAME} ${ZeroMQ_LIBRARIES})

