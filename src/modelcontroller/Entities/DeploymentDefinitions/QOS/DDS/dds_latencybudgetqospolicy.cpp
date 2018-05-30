#include "dds_latencybudgetqospolicy.h"
#include "../../../../entityfactorybroker.h"
#include "../../../../entityfactoryregistrybroker.h"
#include "../../../../entityfactoryregistrybroker.h"

const static NODE_KIND node_kind = NODE_KIND::QOS_DDS_POLICY_LATENCYBUDGET;
const static QString kind_string = "DDS_LatencyBudgetQosPolicy";

void DDS_LatencyBudgetQosPolicy::RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker){
    broker.RegisterWithEntityFactory(node_kind, kind_string, [](EntityFactoryBroker& broker, bool is_temp_node){
        return new DDS_LatencyBudgetQosPolicy(broker, is_temp_node);
    });
}

DDS_LatencyBudgetQosPolicy::DDS_LatencyBudgetQosPolicy(EntityFactoryBroker& broker, bool is_temp) : Node(broker, node_kind, is_temp){
    //Setup State
    setNodeType(NODE_TYPE::QOS);
    setNodeType(NODE_TYPE::DDS);

    if(is_temp){
        //Break out early for temporary entities
        return;
    }

    //Setup Data
    broker.AttachData(this, "label", QVariant::String, ProtectedState::PROTECTED, "latency_budget");
    broker.AttachData(this, "qos_dds_duration_sec", QVariant::String, ProtectedState::UNPROTECTED, "0");
    broker.AttachData(this, "qos_dds_duration_nanosec", QVariant::String, ProtectedState::UNPROTECTED, "0");
}