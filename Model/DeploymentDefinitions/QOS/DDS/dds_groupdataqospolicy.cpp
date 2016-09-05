#include "dds_groupdataqospolicy.h"

DDS_GroupDataQosPolicy::DDS_GroupDataQosPolicy():Node(NK_QOS_DDS_POLICY_GROUPDATA)
{
    setNodeType(NT_QOS); setNodeType(NT_DDS);
}

bool DDS_GroupDataQosPolicy::canAdoptChild(Node*)
{
    return false;
}

bool DDS_GroupDataQosPolicy::canAcceptEdge(Edge::EDGE_KIND edgeKind, Node *dst)
{
    return false;
}
