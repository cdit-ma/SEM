set(PROJ_NAME "re_environment_manager")
project(${PROJ_NAME})

find_package(Boost 1.30.0 COMPONENTS program_options REQUIRED)

add_executable(${PROJ_NAME} "")

target_sources(${PROJ_NAME} PRIVATE
    # Sources
    ${CMAKE_CURRENT_SOURCE_DIR}/attribute.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/component.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/container.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/deploymenthandler.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/deploymentregister.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/environment.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/experiment.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/logger.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/node.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ports/ddsport.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ports/periodicport.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ports/port.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ports/qpidport.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ports/taoport.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ports/zmqport.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/uniquequeue.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/worker.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/environmentmanager.cpp
    
    # Headers
    ${CMAKE_CURRENT_SOURCE_DIR}/attribute.h
    ${CMAKE_CURRENT_SOURCE_DIR}/component.h
    ${CMAKE_CURRENT_SOURCE_DIR}/container.h
    ${CMAKE_CURRENT_SOURCE_DIR}/deploymenthandler.h
    ${CMAKE_CURRENT_SOURCE_DIR}/deploymentregister.h
    ${CMAKE_CURRENT_SOURCE_DIR}/environment.h
    ${CMAKE_CURRENT_SOURCE_DIR}/experiment.h
    ${CMAKE_CURRENT_SOURCE_DIR}/logger.h
    ${CMAKE_CURRENT_SOURCE_DIR}/node.h
    ${CMAKE_CURRENT_SOURCE_DIR}/ports/ddsport.h
    ${CMAKE_CURRENT_SOURCE_DIR}/ports/periodicport.h
    ${CMAKE_CURRENT_SOURCE_DIR}/ports/port.h
    ${CMAKE_CURRENT_SOURCE_DIR}/ports/qpidport.h
    ${CMAKE_CURRENT_SOURCE_DIR}/ports/taoport.h
    ${CMAKE_CURRENT_SOURCE_DIR}/ports/zmqport.h
    ${CMAKE_CURRENT_SOURCE_DIR}/worker.h
    ${CMAKE_CURRENT_BINARY_DIR}/cmakevars.h
)

# Pass Version info into cmakevars.h
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/../cmakevars.h.in ${CMAKE_CURRENT_BINARY_DIR}/cmakevars.h)

# Compile Definitions
target_compile_definitions(${PROJ_NAME} PRIVATE BOOST_ALL_DYN_LINK)
target_include_directories(${PROJ_NAME} PRIVATE ${CMAKE_CURRENT_BINARY_DIR})

# Link Libraries
target_link_libraries(${PROJ_NAME} PRIVATE re_common_proto_control)
target_link_libraries(${PROJ_NAME} PRIVATE re_common_proto_environmentcontrol)
target_link_libraries(${PROJ_NAME} PRIVATE zmq_protoreplier)
target_link_libraries(${PROJ_NAME} PRIVATE zmq_protowriter)
target_link_libraries(${PROJ_NAME} PRIVATE ${Boost_PROGRAM_OPTIONS_LIBRARY})

if(UNIX)
    target_link_libraries(${PROJ_NAME} PRIVATE pthread)
endif(UNIX)