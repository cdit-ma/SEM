set(PROJ_NAME zmq_helper)
project(${PROJ_NAME})

find_package(ZMQ)

if(NOT ZMQ_FOUND)
    message(STATUS "Cannot find ZMQ cannot build project ${PROJ_NAME}")
    return()
endif()
add_library(${PROJ_NAME} SHARED)

target_sources(${PROJ_NAME} PRIVATE
    # Sources
    ${CMAKE_CURRENT_SOURCE_DIR}/zmqhelper.cpp
    # Headers
    ${CMAKE_CURRENT_SOURCE_DIR}/zmqhelper.h
)

# Include Directories
target_include_directories(${PROJ_NAME} PUBLIC ${ZMQ_INCLUDE_DIRS})

# Link Libraries
target_link_libraries(${PROJ_NAME} PUBLIC ${ZMQ_LIBRARIES})