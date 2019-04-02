set(PROJ_NAME zmq_helper)
project(${PROJ_NAME})

find_package(ZMQ)

if(NOT ZMQ_FOUND)
    message(STATUS "Cannot find ZMQ cannot build project ${PROJ_NAME}")
    return()
endif()

add_library(${PROJ_NAME} SHARED "")
target_compile_features(${PROJ_NAME} PRIVATE cxx_std_11)

target_sources(${PROJ_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/zmqhelper.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/zmqhelper.h
)
target_include_directories(${PROJ_NAME} PUBLIC ${ZMQ_INCLUDE_DIRS})
target_link_libraries(${PROJ_NAME} PUBLIC ${ZMQ_LIBRARIES})
