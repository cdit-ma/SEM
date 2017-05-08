#include "dds_latencybudgetqospolicy.h"

DDS_LatencyBudgetQosPolicy::DDS_LatencyBudgetQosPolicy(EntityFactory* factory) : Node(factory, NODE_KIND::QOS_DDS_POLICY_LATENCYBUDGET, "DDS_LatencyBudgetQosPolicy"){
	auto node_kind = NODE_KIND::QOS_DDS_POLICY_LATENCYBUDGET;
	QString kind_string = "DDS_LatencyBudgetQosPolicy";
	RegisterNodeKind(factory, node_kind, kind_string, [](){return new DDS_LatencyBudgetQosPolicy();});

    RegisterDefaultData(factory, node_kind, "label", QVariant::String, true, "latency_budget");
    RegisterDefaultData(factory, node_kind, "qos_dds_duration_sec", QVariant::String, false, "0");
    RegisterDefaultData(factory, node_kind, "qos_dds_duration_nanosec", QVariant::String, false, "0");
}

DDS_LatencyBudgetQosPolicy::DDS_LatencyBudgetQosPolicy():Node(NODE_KIND::QOS_DDS_POLICY_LATENCYBUDGET)
{
    setNodeType(NODE_TYPE::QOS); setNodeType(NODE_TYPE::DDS);
}

bool DDS_LatencyBudgetQosPolicy::canAdoptChild(Node*)
{
    return false;
}

bool DDS_LatencyBudgetQosPolicy::canAcceptEdge(EDGE_KIND, Node *)
{
    return false;
}