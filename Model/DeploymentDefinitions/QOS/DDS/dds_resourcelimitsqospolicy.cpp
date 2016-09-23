#include "dds_resourcelimitsqospolicy.h"

DDS_ResourceLimitsQosPolicy::DDS_ResourceLimitsQosPolicy():Node(NK_QOS_DDS_POLICY_RESOURCELIMITS)
{
    setNodeType(NT_QOS); setNodeType(NT_DDS);
}

bool DDS_ResourceLimitsQosPolicy::canAdoptChild(Node*)
{
    return false;
}

bool DDS_ResourceLimitsQosPolicy::canAcceptEdge(Edge::EDGE_KIND, Node *)
{
    return false;
}
