#include "dds_latencybudgetqospolicy.h"
#include "../../../../entityfactory.h"

const static NODE_KIND node_kind = NODE_KIND::QOS_DDS_POLICY_LATENCYBUDGET;
const static QString kind_string = "DDS_LatencyBudgetQosPolicy";

void DDS_LatencyBudgetQosPolicy::RegisterWithEntityFactory(EntityFactory& factory){
    Node::RegisterWithEntityFactory(factory, node_kind, kind_string, [](EntityFactory& factory, bool is_temp_node){
        return new DDS_LatencyBudgetQosPolicy(factory, is_temp_node);
    });
}

DDS_LatencyBudgetQosPolicy::DDS_LatencyBudgetQosPolicy(EntityFactory& factory, bool is_temp) : Node(factory, node_kind, is_temp){
    if(is_temp){
        return;
    }

    //Setup State
    setNodeType(NODE_TYPE::QOS);
    setNodeType(NODE_TYPE::DDS);

    //Setup Data
    factory.AttachData(this, "label", QVariant::String, "latency_budget", true);
    factory.AttachData(this, "qos_dds_duration_sec", QVariant::String, "0", false);
    factory.AttachData(this, "qos_dds_duration_nanosec", QVariant::String, "0", false);
}