#include "dds_readerdatalifecycleqospolicy.h"

DDS_ReaderDataLifecycleQosPolicy::DDS_ReaderDataLifecycleQosPolicy():Node(NK_QOS_DDS_POLICY_READERDATALIFECYCLE)
{
    setNodeType(NT_QOS); setNodeType(NT_DDS);
}

bool DDS_ReaderDataLifecycleQosPolicy::canAdoptChild(Node*)
{
    return false;
}

bool DDS_ReaderDataLifecycleQosPolicy::canAcceptEdge(Edge::EDGE_KIND, Node *)
{
    return false;
}
