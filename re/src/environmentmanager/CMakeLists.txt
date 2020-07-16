find_package(Boost 1.30.0 COMPONENTS program_options REQUIRED)

add_executable(re_environment_manager)
target_compile_features(re_environment_manager PRIVATE cxx_std_11)

target_sources(re_environment_manager PRIVATE
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
)

# Link Libraries
target_link_libraries(re_environment_manager 
        PRIVATE
        re_common_proto_control
        re_common_proto_environmentcontrol
        zmq_protoreplier
        zmq_protowriter
        sem_version
        execution_helper
        zmq_utils
        Boost::program_options
        )

if(UNIX)
    target_link_libraries(re_environment_manager PRIVATE pthread)
endif(UNIX)