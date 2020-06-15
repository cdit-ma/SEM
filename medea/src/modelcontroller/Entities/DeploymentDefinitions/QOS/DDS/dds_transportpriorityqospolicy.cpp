#include "dds_transportpriorityqospolicy.h"
#include "../../../../entityfactorybroker.h"
#include "../../../../entityfactoryregistrybroker.h"
#include "../../../../entityfactoryregistrybroker.h"

const static NODE_KIND node_kind = NODE_KIND::QOS_DDS_POLICY_TRANSPORTPRIORITY;
const static QString kind_string = "DDS_TransportPriorityQosPolicy";

void DDS_TransportPriorityQosPolicy::RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker){
    broker.RegisterWithEntityFactory(node_kind, kind_string, [](EntityFactoryBroker& broker, bool is_temp_node){
        return new DDS_TransportPriorityQosPolicy(broker, is_temp_node);
    });
}

DDS_TransportPriorityQosPolicy::DDS_TransportPriorityQosPolicy(EntityFactoryBroker& broker, bool is_temp) : Node(broker, node_kind, is_temp){
    //Setup State
    setNodeType(NODE_TYPE::QOS);
    setNodeType(NODE_TYPE::DDS);

    if(is_temp){
        //Break out early for temporary entities
        return;
    }

    //Setup Data
    broker.AttachData(this, KeyName::Label, QVariant::String, ProtectedState::PROTECTED, "transport_priority");
    broker.AttachData(this, KeyName::QosDdsIntValue, QVariant::Int, ProtectedState::UNPROTECTED, 0);
}