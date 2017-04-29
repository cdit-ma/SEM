#include "dds_latencybudgetqospolicy.h"

DDS_LatencyBudgetQosPolicy::DDS_LatencyBudgetQosPolicy():Node(NK_QOS_DDS_POLICY_LATENCYBUDGET)
{
    setNodeType(NT_QOS); setNodeType(NT_DDS);
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