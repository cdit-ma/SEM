cmake_minimum_required (VERSION 2.6)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

#Requires Qt5_DIR to be set to the directory which contains the bin/lib folders for QT5
# IE Qt5_DIR /Users/dan/Qt5.7.0/5.7/clang_64

set(LIB_NAME medea_modelcontroller)
find_package(Qt5 COMPONENTS Core Network REQUIRED)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(SOURCE
    ${CMAKE_CURRENT_SOURCE_DIR}/modelcontroller.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/entityfactory.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/tempentity.cpp
    
    ${CMAKE_CURRENT_SOURCE_DIR}/Entities/graphml.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Entities/key.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Entities/data.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Entities/entity.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Entities/node.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Entities/edge.cpp
    
    ${CMAKE_CURRENT_SOURCE_DIR}/Entities/model.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Entities/workerdefinitions.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/Entities/BehaviourDefinitions/attributeimpl.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Entities/BehaviourDefinitions/behaviourdefinitions.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Entities/BehaviourDefinitions/behaviournode.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Entities/BehaviourDefinitions/branch.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Entities/BehaviourDefinitions/branchstate.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Entities/BehaviourDefinitions/componentimpl.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Entities/BehaviourDefinitions/condition.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Entities/BehaviourDefinitions/eventportimpl.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Entities/BehaviourDefinitions/ineventportimpl.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Entities/BehaviourDefinitions/inputparameter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Entities/BehaviourDefinitions/outeventportimpl.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Entities/BehaviourDefinitions/parameter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Entities/BehaviourDefinitions/periodicevent.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Entities/BehaviourDefinitions/process.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Entities/BehaviourDefinitions/returnparameter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Entities/BehaviourDefinitions/termination.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Entities/BehaviourDefinitions/variable.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Entities/BehaviourDefinitions/whileloop.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Entities/BehaviourDefinitions/workerprocess.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Entities/BehaviourDefinitions/workload.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Entities/DeploymentDefinitions/QOS/DDS/dds_deadlineqospolicy.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Entities/DeploymentDefinitions/QOS/DDS/dds_destinationorderqospolicy.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Entities/DeploymentDefinitions/QOS/DDS/dds_durabilityqospolicy.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Entities/DeploymentDefinitions/QOS/DDS/dds_durabilityserviceqospolicy.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Entities/DeploymentDefinitions/QOS/DDS/dds_entityfactoryqospolicy.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Entities/DeploymentDefinitions/QOS/DDS/dds_groupdataqospolicy.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Entities/DeploymentDefinitions/QOS/DDS/dds_historyqospolicy.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Entities/DeploymentDefinitions/QOS/DDS/dds_latencybudgetqospolicy.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Entities/DeploymentDefinitions/QOS/DDS/dds_lifespanqospolicy.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Entities/DeploymentDefinitions/QOS/DDS/dds_livelinessqospolicy.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Entities/DeploymentDefinitions/QOS/DDS/dds_ownershipqospolicy.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Entities/DeploymentDefinitions/QOS/DDS/dds_ownershipstrengthqospolicy.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Entities/DeploymentDefinitions/QOS/DDS/dds_partitionqospolicy.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Entities/DeploymentDefinitions/QOS/DDS/dds_presentationqospolicy.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Entities/DeploymentDefinitions/QOS/DDS/dds_qosprofile.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Entities/DeploymentDefinitions/QOS/DDS/dds_readerdatalifecycleqospolicy.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Entities/DeploymentDefinitions/QOS/DDS/dds_reliabilityqospolicy.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Entities/DeploymentDefinitions/QOS/DDS/dds_resourcelimitsqospolicy.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Entities/DeploymentDefinitions/QOS/DDS/dds_timebasedfilterqospolicy.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Entities/DeploymentDefinitions/QOS/DDS/dds_topicdataqospolicy.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Entities/DeploymentDefinitions/QOS/DDS/dds_transportpriorityqospolicy.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Entities/DeploymentDefinitions/QOS/DDS/dds_userdataqospolicy.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Entities/DeploymentDefinitions/QOS/DDS/dds_writerdatalifecycleqospolicy.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Entities/DeploymentDefinitions/assemblydefinitions.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Entities/DeploymentDefinitions/attributeinstance.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Entities/DeploymentDefinitions/blackboxinstance.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Entities/DeploymentDefinitions/componentassembly.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Entities/DeploymentDefinitions/componentinstance.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Entities/DeploymentDefinitions/deploymentdefinitions.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Entities/DeploymentDefinitions/eventportdelegate.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Entities/DeploymentDefinitions/hardwarecluster.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Entities/DeploymentDefinitions/hardwaredefinitions.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Entities/DeploymentDefinitions/hardwarenode.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Entities/DeploymentDefinitions/ineventportdelegate.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Entities/DeploymentDefinitions/ineventportinstance.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Entities/DeploymentDefinitions/managementcomponent.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Entities/DeploymentDefinitions/outeventportdelegate.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Entities/DeploymentDefinitions/outeventportinstance.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Entities/Edges/aggregateedge.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Entities/Edges/assemblyedge.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Entities/Edges/dataedge.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Entities/Edges/definitionedge.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Entities/Edges/deploymentedge.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Entities/Edges/qosedge.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Entities/Edges/workflowedge.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Entities/InterfaceDefinitions/aggregate.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Entities/InterfaceDefinitions/aggregateinstance.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Entities/InterfaceDefinitions/attribute.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Entities/InterfaceDefinitions/blackbox.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Entities/InterfaceDefinitions/component.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Entities/InterfaceDefinitions/datanode.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Entities/InterfaceDefinitions/eventport.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Entities/InterfaceDefinitions/idl.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Entities/InterfaceDefinitions/ineventport.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Entities/InterfaceDefinitions/interfacedefinitions.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Entities/InterfaceDefinitions/member.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Entities/InterfaceDefinitions/memberinstance.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Entities/InterfaceDefinitions/outeventport.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Entities/InterfaceDefinitions/vector.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Entities/InterfaceDefinitions/vectorinstance.cpp
    
    ${CMAKE_CURRENT_SOURCE_DIR}/Entities/BehaviourDefinitions/variadicparameter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Entities/BehaviourDefinitions/code.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Entities/BehaviourDefinitions/header.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Entities/BehaviourDefinitions/forcondition.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Entities/BehaviourDefinitions/setter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Entities/BehaviourDefinitions/variableparameter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Entities/DeploymentDefinitions/loggingprofile.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Entities/DeploymentDefinitions/loggingserver.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Entities/Keys/labelkey.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Entities/Keys/indexkey.cpp
    
)

set(HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/modelcontroller.h
    ${CMAKE_CURRENT_SOURCE_DIR}/entityfactory.h
    ${CMAKE_CURRENT_SOURCE_DIR}/tempentity.h
    
    ${CMAKE_CURRENT_SOURCE_DIR}/Entities/graphml.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Entities/key.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Entities/data.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Entities/entity.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Entities/node.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Entities/edge.h
    
    ${CMAKE_CURRENT_SOURCE_DIR}/Entities/model.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Entities/workerdefinitions.h

    ${CMAKE_CURRENT_SOURCE_DIR}/Entities/BehaviourDefinitions/attributeimpl.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Entities/BehaviourDefinitions/behaviourdefinitions.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Entities/BehaviourDefinitions/behaviournode.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Entities/BehaviourDefinitions/branch.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Entities/BehaviourDefinitions/branchstate.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Entities/BehaviourDefinitions/componentimpl.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Entities/BehaviourDefinitions/condition.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Entities/BehaviourDefinitions/eventportimpl.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Entities/BehaviourDefinitions/ineventportimpl.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Entities/BehaviourDefinitions/inputparameter.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Entities/BehaviourDefinitions/outeventportimpl.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Entities/BehaviourDefinitions/parameter.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Entities/BehaviourDefinitions/periodicevent.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Entities/BehaviourDefinitions/process.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Entities/BehaviourDefinitions/returnparameter.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Entities/BehaviourDefinitions/termination.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Entities/BehaviourDefinitions/variable.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Entities/BehaviourDefinitions/whileloop.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Entities/BehaviourDefinitions/workerprocess.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Entities/BehaviourDefinitions/workload.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Entities/DeploymentDefinitions/QOS/DDS/dds_deadlineqospolicy.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Entities/DeploymentDefinitions/QOS/DDS/dds_destinationorderqospolicy.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Entities/DeploymentDefinitions/QOS/DDS/dds_durabilityqospolicy.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Entities/DeploymentDefinitions/QOS/DDS/dds_durabilityserviceqospolicy.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Entities/DeploymentDefinitions/QOS/DDS/dds_entityfactoryqospolicy.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Entities/DeploymentDefinitions/QOS/DDS/dds_groupdataqospolicy.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Entities/DeploymentDefinitions/QOS/DDS/dds_historyqospolicy.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Entities/DeploymentDefinitions/QOS/DDS/dds_latencybudgetqospolicy.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Entities/DeploymentDefinitions/QOS/DDS/dds_lifespanqospolicy.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Entities/DeploymentDefinitions/QOS/DDS/dds_livelinessqospolicy.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Entities/DeploymentDefinitions/QOS/DDS/dds_ownershipqospolicy.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Entities/DeploymentDefinitions/QOS/DDS/dds_ownershipstrengthqospolicy.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Entities/DeploymentDefinitions/QOS/DDS/dds_partitionqospolicy.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Entities/DeploymentDefinitions/QOS/DDS/dds_presentationqospolicy.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Entities/DeploymentDefinitions/QOS/DDS/dds_qosprofile.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Entities/DeploymentDefinitions/QOS/DDS/dds_readerdatalifecycleqospolicy.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Entities/DeploymentDefinitions/QOS/DDS/dds_reliabilityqospolicy.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Entities/DeploymentDefinitions/QOS/DDS/dds_resourcelimitsqospolicy.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Entities/DeploymentDefinitions/QOS/DDS/dds_timebasedfilterqospolicy.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Entities/DeploymentDefinitions/QOS/DDS/dds_topicdataqospolicy.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Entities/DeploymentDefinitions/QOS/DDS/dds_transportpriorityqospolicy.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Entities/DeploymentDefinitions/QOS/DDS/dds_userdataqospolicy.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Entities/DeploymentDefinitions/QOS/DDS/dds_writerdatalifecycleqospolicy.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Entities/DeploymentDefinitions/assemblydefinitions.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Entities/DeploymentDefinitions/attributeinstance.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Entities/DeploymentDefinitions/blackboxinstance.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Entities/DeploymentDefinitions/componentassembly.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Entities/DeploymentDefinitions/componentinstance.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Entities/DeploymentDefinitions/deploymentdefinitions.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Entities/DeploymentDefinitions/eventportdelegate.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Entities/DeploymentDefinitions/hardwarecluster.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Entities/DeploymentDefinitions/hardwaredefinitions.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Entities/DeploymentDefinitions/hardwarenode.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Entities/DeploymentDefinitions/ineventportdelegate.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Entities/DeploymentDefinitions/ineventportinstance.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Entities/DeploymentDefinitions/managementcomponent.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Entities/DeploymentDefinitions/outeventportdelegate.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Entities/DeploymentDefinitions/outeventportinstance.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Entities/Edges/aggregateedge.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Entities/Edges/assemblyedge.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Entities/Edges/dataedge.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Entities/Edges/definitionedge.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Entities/Edges/deploymentedge.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Entities/Edges/qosedge.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Entities/Edges/workflowedge.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Entities/InterfaceDefinitions/aggregate.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Entities/InterfaceDefinitions/aggregateinstance.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Entities/InterfaceDefinitions/attribute.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Entities/InterfaceDefinitions/blackbox.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Entities/InterfaceDefinitions/component.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Entities/InterfaceDefinitions/datanode.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Entities/InterfaceDefinitions/eventport.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Entities/InterfaceDefinitions/idl.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Entities/InterfaceDefinitions/ineventport.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Entities/InterfaceDefinitions/interfacedefinitions.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Entities/InterfaceDefinitions/member.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Entities/InterfaceDefinitions/memberinstance.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Entities/InterfaceDefinitions/outeventport.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Entities/InterfaceDefinitions/vector.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Entities/InterfaceDefinitions/vectorinstance.h
    
    ${CMAKE_CURRENT_SOURCE_DIR}/Entities/BehaviourDefinitions/variadicparameter.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Entities/BehaviourDefinitions/code.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Entities/BehaviourDefinitions/header.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Entities/BehaviourDefinitions/forcondition.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Entities/BehaviourDefinitions/setter.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Entities/BehaviourDefinitions/variableparameter.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Entities/DeploymentDefinitions/loggingprofile.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Entities/DeploymentDefinitions/loggingserver.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Entities/Keys/labelkey.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Entities/Keys/indexkey.h
)

set(RESOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/workers.qrc
    )

add_library(${LIB_NAME} STATIC ${SOURCE} ${HEADERS} ${RESOURCES})

target_link_libraries(${LIB_NAME} Qt5::Core)
target_link_libraries(${LIB_NAME} Qt5::Network)