#include "dds_presentationqospolicy.h"
#include "../../../../entityfactorybroker.h"
#include "../../../../entityfactoryregistrybroker.h"
#include "../../../../entityfactoryregistrybroker.h"

const static NODE_KIND node_kind = NODE_KIND::QOS_DDS_POLICY_PRESENTATION;
const static QString kind_string = "DDS_PresentationQosPolicy";

void DDS_PresentationQosPolicy::RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker){
    broker.RegisterWithEntityFactory(node_kind, kind_string, [](EntityFactoryBroker& broker, bool is_temp_node){
        return new DDS_PresentationQosPolicy(broker, is_temp_node);
    });
}

DDS_PresentationQosPolicy::DDS_PresentationQosPolicy(EntityFactoryBroker& broker, bool is_temp) : Node(broker, node_kind, is_temp){
    //Setup State
    setNodeType(NODE_TYPE::QOS);
    setNodeType(NODE_TYPE::DDS);

    if(is_temp){
        //Break out early for temporary entities
        return;
    }

    //Setup Data
    broker.AttachData(this, KeyName::Label, QVariant::String, ProtectedState::PROTECTED, "presentation");
    broker.AttachData(this, KeyName::QosDdsCoherentAccess, QVariant::Bool, ProtectedState::UNPROTECTED, false);
    broker.AttachData(this, KeyName::QosDdsOrderedAccess, QVariant::Bool, ProtectedState::UNPROTECTED, false);
    
    auto dds_scope_data = broker.AttachData(this, KeyName::QosDdsAccessScope, QVariant::String, ProtectedState::UNPROTECTED);
    dds_scope_data->addValidValues({"INSTANCE_PRESENTATION_QOS", "TOPIC_PRESENTATION_QOS", "GROUP_PRESENTATION_QOS"});
}