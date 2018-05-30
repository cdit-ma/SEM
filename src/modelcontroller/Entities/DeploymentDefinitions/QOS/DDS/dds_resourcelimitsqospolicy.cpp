#include "dds_resourcelimitsqospolicy.h"
#include "../../../../entityfactorybroker.h"
#include "../../../../entityfactoryregistrybroker.h"
#include "../../../../entityfactoryregistrybroker.h"

const static NODE_KIND node_kind = NODE_KIND::QOS_DDS_POLICY_RESOURCELIMITS;
const static QString kind_string = "DDS_ResourceLimitsQosPolicy";

void DDS_ResourceLimitsQosPolicy::RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker){
    broker.RegisterWithEntityFactory(node_kind, kind_string, [](EntityFactoryBroker& broker, bool is_temp_node){
        return new DDS_ResourceLimitsQosPolicy(broker, is_temp_node);
    });
}

DDS_ResourceLimitsQosPolicy::DDS_ResourceLimitsQosPolicy(EntityFactoryBroker& broker, bool is_temp) : Node(broker, node_kind, is_temp){
    //Setup State
    setNodeType(NODE_TYPE::QOS);
    setNodeType(NODE_TYPE::DDS);

    if(is_temp){
        //Break out early for temporary entities
        return;
    }

    //Setup Data
    broker.AttachData(this, "label", QVariant::String, ProtectedState::PROTECTED, "resource_limits");
    broker.AttachData(this, "qos_dds_max_samples", QVariant::String, ProtectedState::UNPROTECTED, "LENGTH_UNLIMITED");
    broker.AttachData(this, "qos_dds_max_instances", QVariant::String, ProtectedState::UNPROTECTED, "LENGTH_UNLIMITED");
    broker.AttachData(this, "qos_dds_max_samples_per_instance", QVariant::String, ProtectedState::UNPROTECTED, "LENGTH_UNLIMITED");
}