#include "dds_latencybudgetqospolicy.h"

DDS_LatencyBudgetQosPolicy::DDS_LatencyBudgetQosPolicy():Node(NK_QOS_DDS_POLICY_LATENCYBUDGET)
{
    setNodeType(NT_QOS_DDS_POLICY);
}

bool DDS_LatencyBudgetQosPolicy::canAdoptChild(Node*)
{
    return false;
}

bool DDS_LatencyBudgetQosPolicy::canAcceptEdge(Edge::EDGE_CLASS edgeKind, Node *dst)
{
    return false;
}
