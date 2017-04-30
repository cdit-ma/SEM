#include "dds_latencybudgetqospolicy.h"

DDS_LatencyBudgetQosPolicy::DDS_LatencyBudgetQosPolicy():Node(NODE_KIND::QOS_DDS_POLICY_LATENCYBUDGET)
{
    setNodeType(NODE_TYPE::QOS); setNodeType(NODE_TYPE::DDS);
    setMoveEnabled(false);
    setExpandEnabled(false);
    updateDefaultData("label", QVariant::String, true, "latency_budget");
    updateDefaultData("qos_dds_duration_sec", QVariant::String, false, "0");
    updateDefaultData("qos_dds_duration_nanosec", QVariant::String, false, "0");
}

bool DDS_LatencyBudgetQosPolicy::canAdoptChild(Node*)
{
    return false;
}

bool DDS_LatencyBudgetQosPolicy::canAcceptEdge(Edge::EDGE_KIND, Node *)
{
    return false;
}