#include "dds_durabilityserviceqospolicy.h"
#include "../../../../entityfactorybroker.h"
#include "../../../../entityfactoryregistrybroker.h"
#include "../../../../entityfactoryregistrybroker.h"

const static NODE_KIND node_kind = NODE_KIND::QOS_DDS_POLICY_DURABILITYSERVICE;
const static QString kind_string = "DDS_DurabilityServiceQosPolicy";

void DDS_DurabilityServiceQosPolicy::RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker){
    broker.RegisterWithEntityFactory(node_kind, kind_string, [](EntityFactoryBroker& broker, bool is_temp_node){
        return new DDS_DurabilityServiceQosPolicy(broker, is_temp_node);
    });
}

DDS_DurabilityServiceQosPolicy::DDS_DurabilityServiceQosPolicy(EntityFactoryBroker& broker, bool is_temp) : Node(broker, node_kind, is_temp){
    if(is_temp){
        return;
    }

    //Setup State
    setNodeType(NODE_TYPE::QOS);
    setNodeType(NODE_TYPE::DDS);
    
    //Setup Data
    broker.AttachData(this, KeyName::Label, QVariant::String, ProtectedState::PROTECTED, "durability_service");
    broker.AttachData(this, KeyName::QosDdsServiceCleanupDelaySec, QVariant::String, ProtectedState::UNPROTECTED, "0");
    broker.AttachData(this, KeyName::QosDdsServiceCleanupDelayNanoSec, QVariant::String, ProtectedState::UNPROTECTED, "0");
    broker.AttachData(this, KeyName::QosDdsHistoryDepth, QVariant::String, ProtectedState::UNPROTECTED, "1");
    broker.AttachData(this, KeyName::QosDdsMaxSamples, QVariant::String, ProtectedState::UNPROTECTED, "LENGTH_UNLIMITED");
    broker.AttachData(this, KeyName::QosDdsMaxInstances, QVariant::String, ProtectedState::UNPROTECTED,  "LENGTH_UNLIMITED");
    broker.AttachData(this, KeyName::QosDdsMaxSamplesPerInstance, QVariant::String, ProtectedState::UNPROTECTED,  "LENGTH_UNLIMITED");
    
    auto dds_kind_data = broker.AttachData(this, KeyName::QosDdsHistoryKind, QVariant::String, ProtectedState::UNPROTECTED);
    dds_kind_data->addValidValues({"KEEP_LAST_HISTORY_QOS", "KEEP_ALL_HISTORY_QOS"});
}