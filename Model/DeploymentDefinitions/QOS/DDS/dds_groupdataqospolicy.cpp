#include "dds_groupdataqospolicy.h"

DDS_GroupDataQosPolicy::DDS_GroupDataQosPolicy():Node(NK_QOS_DDS_POLICY_GROUPDATA)
{
    setNodeType(NT_QOS_DDS_POLICY);
}

bool DDS_GroupDataQosPolicy::canAdoptChild(Node*)
{
    return false;
}

bool DDS_GroupDataQosPolicy::canAcceptEdge(Edge::EDGE_CLASS edgeKind, Node *dst)
{
    return false;
}
