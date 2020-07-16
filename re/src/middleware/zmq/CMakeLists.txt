set(PROJ_NAME zmq_helper)
project(${PROJ_NAME}
        VERSION ${PROJECT_VERSION})


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
target_link_libraries(${PROJ_NAME} PUBLIC cppzmq)
