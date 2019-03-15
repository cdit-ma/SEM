set(PROJ_NAME zmq_helper)
project(${PROJ_NAME})

find_package(ZMQ)

if(NOT ZMQ_FOUND)
    message(STATUS "Cannot find ZMQ cannot build project ${PROJ_NAME}")
    return()
endif()

set(SOURCE
    ${CMAKE_CURRENT_SOURCE_DIR}/zmqhelper.cpp
    )

set(HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/zmqhelper.h
    )

add_library(${PROJ_NAME} SHARED ${SOURCE} ${HEADERS})

# Include Directories
target_include_directories(${PROJ_NAME} PUBLIC ${ZMQ_INCLUDE_DIRS})

# Link Libraries
target_link_libraries(${PROJ_NAME} PUBLIC ${ZMQ_LIBRARIES})