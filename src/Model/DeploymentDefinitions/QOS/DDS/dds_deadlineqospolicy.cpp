#include "dds_deadlineqospolicy.h"

DDS_DeadlineQosPolicy::DDS_DeadlineQosPolicy():Node(NK_QOS_DDS_POLICY_DEADLINE)
{
    setNodeType(NT_QOS); setNodeType(NT_DDS);
}

bool DDS_DeadlineQosPolicy::canAdoptChild(Node*)
{
    return false;
}

bool DDS_DeadlineQosPolicy::canAcceptEdge(Edge::EDGE_KIND edgeKind, Node *dst)
{
    Q_UNUSED(edgeKind);
    Q_UNUSED(dst);
    return false;
}
