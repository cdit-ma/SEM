set(PROJ_NAME zmq_helper)
project(${PROJ_NAME}
        VERSION ${PROJECT_VERSION})

add_library(${PROJ_NAME} SHARED "")
target_compile_features(${PROJ_NAME} PRIVATE cxx_std_11)

target_sources(${PROJ_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/zmqhelper.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/zmqhelper.h
)
target_link_libraries(${PROJ_NAME} PUBLIC cppzmq)
target_include_directories(${PROJ_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_library(zmq_ports INTERFACE)
target_sources(zmq_ports
        INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}/pubsub/publisherport.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/pubsub/subscriberport.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/requestreply/requesterport.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/requestreply/replierport.hpp
        )
target_include_directories(zmq_ports INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(zmq_ports
        INTERFACE
        zmq_helper
        zmq_utils
        prototranslator
        )