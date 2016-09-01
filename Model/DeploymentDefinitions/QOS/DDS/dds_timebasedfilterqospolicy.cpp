#include "dds_timebasedfilterqospolicy.h"

DDS_TimeBasedFilterQosPolicy::DDS_TimeBasedFilterQosPolicy():Node(NK_QOS_DDS_POLICY_TIMEBASEDFILTER)
{
    setNodeType(NT_QOS); setNodeType(NT_DDS);
}

bool DDS_TimeBasedFilterQosPolicy::canAdoptChild(Node*)
{
    return false;
}

bool DDS_TimeBasedFilterQosPolicy::canAcceptEdge(Edge::EDGE_CLASS edgeKind, Node *dst)
{
    return false;
}
