#include "dds_transportpriorityqospolicy.h"

DDS_TransportPriorityQosPolicy::DDS_TransportPriorityQosPolicy():Node(NK_QOS_DDS_POLICY_TRANSPORTPRIORITY)
{
    setNodeType(NT_QOS); setNodeType(NT_DDS);
}

bool DDS_TransportPriorityQosPolicy::canAdoptChild(Node*)
{
    return false;
}

bool DDS_TransportPriorityQosPolicy::canAcceptEdge(Edge::EDGE_CLASS edgeKind, Node *dst)
{
    return false;
}
