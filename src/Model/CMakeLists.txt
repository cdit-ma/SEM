cmake_minimum_required (VERSION 2.6)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

set(LIB_NAME MEDEA_MODELCONTROLLER)
find_package(Qt5Core)
find_package(Qt5Network)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)



set(SOURCE
    ${CMAKE_CURRENT_SOURCE_DIR}/../Controllers/modelcontroller.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/BehaviourDefinitions/attributeimpl.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/BehaviourDefinitions/behaviourdefinitions.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/BehaviourDefinitions/behaviournode.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/BehaviourDefinitions/branch.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/BehaviourDefinitions/branchstate.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/BehaviourDefinitions/componentimpl.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/BehaviourDefinitions/condition.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/BehaviourDefinitions/eventportimpl.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/BehaviourDefinitions/ineventportimpl.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/BehaviourDefinitions/inputparameter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/BehaviourDefinitions/outeventportimpl.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/BehaviourDefinitions/parameter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/BehaviourDefinitions/periodicevent.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/BehaviourDefinitions/process.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/BehaviourDefinitions/returnparameter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/BehaviourDefinitions/termination.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/BehaviourDefinitions/variable.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/BehaviourDefinitions/whileloop.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/BehaviourDefinitions/workerprocess.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/BehaviourDefinitions/workload.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/DeploymentDefinitions/QOS/DDS/dds_deadlineqospolicy.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/DeploymentDefinitions/QOS/DDS/dds_destinationorderqospolicy.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/DeploymentDefinitions/QOS/DDS/dds_durabilityqospolicy.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/DeploymentDefinitions/QOS/DDS/dds_durabilityserviceqospolicy.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/DeploymentDefinitions/QOS/DDS/dds_entityfactoryqospolicy.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/DeploymentDefinitions/QOS/DDS/dds_groupdataqospolicy.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/DeploymentDefinitions/QOS/DDS/dds_historyqospolicy.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/DeploymentDefinitions/QOS/DDS/dds_latencybudgetqospolicy.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/DeploymentDefinitions/QOS/DDS/dds_lifespanqospolicy.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/DeploymentDefinitions/QOS/DDS/dds_livelinessqospolicy.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/DeploymentDefinitions/QOS/DDS/dds_ownershipqospolicy.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/DeploymentDefinitions/QOS/DDS/dds_ownershipstrengthqospolicy.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/DeploymentDefinitions/QOS/DDS/dds_partitionqospolicy.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/DeploymentDefinitions/QOS/DDS/dds_presentationqospolicy.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/DeploymentDefinitions/QOS/DDS/dds_qosprofile.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/DeploymentDefinitions/QOS/DDS/dds_readerdatalifecycleqospolicy.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/DeploymentDefinitions/QOS/DDS/dds_reliabilityqospolicy.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/DeploymentDefinitions/QOS/DDS/dds_resourcelimitsqospolicy.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/DeploymentDefinitions/QOS/DDS/dds_timebasedfilterqospolicy.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/DeploymentDefinitions/QOS/DDS/dds_topicdataqospolicy.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/DeploymentDefinitions/QOS/DDS/dds_transportpriorityqospolicy.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/DeploymentDefinitions/QOS/DDS/dds_userdataqospolicy.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/DeploymentDefinitions/QOS/DDS/dds_writerdatalifecycleqospolicy.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/DeploymentDefinitions/assemblydefinitions.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/DeploymentDefinitions/attributeinstance.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/DeploymentDefinitions/blackboxinstance.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/DeploymentDefinitions/componentassembly.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/DeploymentDefinitions/componentinstance.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/DeploymentDefinitions/deploymentdefinitions.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/DeploymentDefinitions/eventportdelegate.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/DeploymentDefinitions/hardwarecluster.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/DeploymentDefinitions/hardwaredefinitions.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/DeploymentDefinitions/hardwarenode.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/DeploymentDefinitions/ineventportdelegate.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/DeploymentDefinitions/ineventportinstance.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/DeploymentDefinitions/managementcomponent.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/DeploymentDefinitions/outeventportdelegate.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/DeploymentDefinitions/outeventportinstance.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Edges/aggregateedge.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Edges/assemblyedge.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Edges/dataedge.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Edges/definitionedge.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Edges/deploymentedge.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Edges/qosedge.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Edges/workflowedge.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/InterfaceDefinitions/aggregate.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/InterfaceDefinitions/aggregateinstance.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/InterfaceDefinitions/attribute.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/InterfaceDefinitions/blackbox.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/InterfaceDefinitions/component.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/InterfaceDefinitions/datanode.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/InterfaceDefinitions/eventport.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/InterfaceDefinitions/idl.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/InterfaceDefinitions/ineventport.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/InterfaceDefinitions/interfacedefinitions.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/InterfaceDefinitions/member.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/InterfaceDefinitions/memberinstance.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/InterfaceDefinitions/outeventport.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/InterfaceDefinitions/vector.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/InterfaceDefinitions/vectorinstance.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/data.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/edge.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/entity.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/graphml.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/key.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/model.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/node.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/tempentity.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/workerdefinitions.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/BehaviourDefinitions/variadicparameter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/BehaviourDefinitions/code.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/BehaviourDefinitions/header.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/BehaviourDefinitions/forcondition.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/BehaviourDefinitions/setter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/BehaviourDefinitions/variableparameter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/DeploymentDefinitions/loggingprofile.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/DeploymentDefinitions/loggingserver.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Keys/labelkey.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Keys/indexkey.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/entityfactory.cpp
)

SET(HEADERS
    
)

set(RESOURCES
    )



add_library(${LIB_NAME} STATIC ${SOURCE} ${HEADERS} ${RESOURCES})

target_link_libraries(${LIB_NAME} Qt5::Core)

target_link_libraries(${LIB_NAME} Qt5::Network)