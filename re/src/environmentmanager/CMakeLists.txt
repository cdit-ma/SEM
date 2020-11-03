find_package(Boost 1.30.0 COMPONENTS program_options REQUIRED)

add_executable(re_environment_manager)
target_compile_features(re_environment_manager PRIVATE cxx_std_17)
target_compile_definitions(re_environment_manager PRIVATE BOOST_ALL_DYN_LINK)

target_sources(re_environment_manager PRIVATE
        # Sources
#        ${CMAKE_CURRENT_SOURCE_DIR}/environment/attribute.cpp
#        ${CMAKE_CURRENT_SOURCE_DIR}/environment/component.cpp
#        ${CMAKE_CURRENT_SOURCE_DIR}/environment/container.cpp
#        ${CMAKE_CURRENT_SOURCE_DIR}/deploymenthandler.cpp
#        ${CMAKE_CURRENT_SOURCE_DIR}/deploymentregister.cpp
#        ${CMAKE_CURRENT_SOURCE_DIR}/environment/environment.cpp
#        ${CMAKE_CURRENT_SOURCE_DIR}/environment/experiment.cpp
#        ${CMAKE_CURRENT_SOURCE_DIR}/environment/logger.cpp
#        ${CMAKE_CURRENT_SOURCE_DIR}/environment/node.cpp
#        ${CMAKE_CURRENT_SOURCE_DIR}/environment/ports/ddsport.cpp
#        ${CMAKE_CURRENT_SOURCE_DIR}/environment/ports/periodicport.cpp
#        ${CMAKE_CURRENT_SOURCE_DIR}/environment/ports/port.cpp
#        ${CMAKE_CURRENT_SOURCE_DIR}/environment/ports/qpidport.cpp
#        ${CMAKE_CURRENT_SOURCE_DIR}/environment/ports/taoport.cpp
#        ${CMAKE_CURRENT_SOURCE_DIR}/environment/ports/zmqport.cpp
#        ${CMAKE_CURRENT_SOURCE_DIR}/environment/worker.cpp
#        ${CMAKE_CURRENT_SOURCE_DIR}/environment/triggerprototype.cpp
#        ${CMAKE_CURRENT_SOURCE_DIR}/endpointtracker.cpp
#        ${CMAKE_CURRENT_SOURCE_DIR}/experimentmanager.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/nodemanagerregistryimpl.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/environmentmanager.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/nodemanagerregistrarimpl.cpp


        # Headers
#        ${CMAKE_CURRENT_SOURCE_DIR}/environment/attribute.h
#        ${CMAKE_CURRENT_SOURCE_DIR}/environment/component.h
#        ${CMAKE_CURRENT_SOURCE_DIR}/environment/container.h
#        ${CMAKE_CURRENT_SOURCE_DIR}/deploymenthandler.h
#        ${CMAKE_CURRENT_SOURCE_DIR}/deploymentregister.h
#        ${CMAKE_CURRENT_SOURCE_DIR}/environment/environment.h
#        ${CMAKE_CURRENT_SOURCE_DIR}/environment/experiment.h
#        ${CMAKE_CURRENT_SOURCE_DIR}/environment/logger.h
#        ${CMAKE_CURRENT_SOURCE_DIR}/environment/node.h
#        ${CMAKE_CURRENT_SOURCE_DIR}/environment/ports/ddsport.h
#        ${CMAKE_CURRENT_SOURCE_DIR}/environment/ports/periodicport.h
#        ${CMAKE_CURRENT_SOURCE_DIR}/environment/ports/port.h
#        ${CMAKE_CURRENT_SOURCE_DIR}/environment/ports/qpidport.h
#        ${CMAKE_CURRENT_SOURCE_DIR}/environment/ports/taoport.h
#        ${CMAKE_CURRENT_SOURCE_DIR}/environment/ports/zmqport.h
#        ${CMAKE_CURRENT_SOURCE_DIR}/environment/worker.h
#        ${CMAKE_CURRENT_SOURCE_DIR}/environment/triggerprototype.h
#        ${CMAKE_CURRENT_SOURCE_DIR}/endpointtracker.h
#        ${CMAKE_CURRENT_SOURCE_DIR}/experimentmanager.h
        ${CMAKE_CURRENT_SOURCE_DIR}/nodemanagerregistry.h
        ${CMAKE_CURRENT_SOURCE_DIR}/nodemanagerregistryimpl.h
        ${CMAKE_CURRENT_SOURCE_DIR}/nodemanagerregistrarimpl.h
        )

# Link Libraries
target_link_libraries(re_environment_manager
        PRIVATE
        node_manager_registration_service
#        node_manager_control_service
        sem_grpc_util
        sem_version
        Boost::program_options
#        re_common_proto_control
#        re_network
#        re_loggers
#        re_common_proto_environmentcontrol
#        zmq_protoreplier
#        zmq_protowriter
#        execution_helper
#        zmq_utils
#        node_manager_registration_protocol
#        node_manager_control_protocol
#        epm_registration_protocol
#        epm_control_protocol
#        epm_stdout_protocol
#        trigger_event_protocol
#        experiment_definition_protocol
#        experiment_definition
#        zmq_protoreplier
#        zmq_protowriter
#        docker-cpp
        PUBLIC
#        zmq_environmentrequester
        )

if (UNIX)
    target_link_libraries(re_environment_manager PRIVATE pthread)
endif (UNIX)