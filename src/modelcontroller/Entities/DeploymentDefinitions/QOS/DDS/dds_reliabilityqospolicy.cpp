#include "dds_reliabilityqospolicy.h"
#include "../../../../entityfactorybroker.h"
#include "../../../../entityfactoryregistrybroker.h"
#include "../../../../entityfactoryregistrybroker.h"

const static NODE_KIND node_kind = NODE_KIND::QOS_DDS_POLICY_RELIABILITY;
const static QString kind_string = "DDS_ReliabilityQosPolicy";

void DDS_ReliabilityQosPolicy::RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker){
    broker.RegisterWithEntityFactory(node_kind, kind_string, [](EntityFactoryBroker& broker, bool is_temp_node){
        return new DDS_ReliabilityQosPolicy(broker, is_temp_node);
    });
}

DDS_ReliabilityQosPolicy::DDS_ReliabilityQosPolicy(EntityFactoryBroker& broker, bool is_temp) : Node(broker, node_kind, is_temp){
    //Setup State
    setNodeType(NODE_TYPE::QOS);
    setNodeType(NODE_TYPE::DDS);

    if(is_temp){
        //Break out early for temporary entities
        return;
    }

    //Setup Data
    broker.AttachData(this, KeyName::Label, QVariant::String, ProtectedState::PROTECTED, "reliability");
    broker.AttachData(this, KeyName::QosDdsMaxBlockingTimeSec, QVariant::String, ProtectedState::UNPROTECTED, "0");
    broker.AttachData(this, KeyName::QosDdsMaxBlockingTimeNanoSec, QVariant::String, ProtectedState::UNPROTECTED, "100000000");
    
    auto dds_kinds_data = broker.AttachData(this, KeyName::QosDdsKind, QVariant::String, ProtectedState::UNPROTECTED);
    dds_kinds_data->addValidValues({"BEST_EFFORT_RELIABILITY_QOS", "RELIABLE_RELIABILITY_QOS"});
}