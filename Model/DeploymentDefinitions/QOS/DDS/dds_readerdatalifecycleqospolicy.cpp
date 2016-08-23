#include "dds_readerdatalifecycleqospolicy.h"

DDS_ReaderDataLifecycleQosPolicy::DDS_ReaderDataLifecycleQosPolicy():Node(NK_QOS_DDS_POLICY_READERDATALIFECYCLE)
{
    setNodeType(NT_QOS_DDS_POLICY);
}

bool DDS_ReaderDataLifecycleQosPolicy::canAdoptChild(Node*)
{
    return false;
}

bool DDS_ReaderDataLifecycleQosPolicy::canAcceptEdge(Edge::EDGE_CLASS edgeKind, Node *dst)
{
    return false;
}
