#include "dds_deadlineqospolicy.h"

DDS_DeadlineQosPolicy::DDS_DeadlineQosPolicy():Node(NK_QOS_DDS_POLICY_DEADLINE)
{
    setNodeType(NT_QOS); setNodeType(NT_DDS);
}

bool DDS_DeadlineQosPolicy::canAdoptChild(Node*)
{
    return false;
}

bool DDS_DeadlineQosPolicy::canAcceptEdge(Edge::EDGE_CLASS edgeKind, Node *dst)
{
    return false;
}
