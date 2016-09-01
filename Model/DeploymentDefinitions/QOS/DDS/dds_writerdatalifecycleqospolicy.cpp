#include "dds_writerdatalifecycleqospolicy.h"

DDS_WriterDataLifecycleQosPolicy::DDS_WriterDataLifecycleQosPolicy():Node(NK_QOS_DDS_POLICY_WRITERDATALIFECYCLE)
{
    setNodeType(NT_QOS); setNodeType(NT_DDS);
}

bool DDS_WriterDataLifecycleQosPolicy::canAdoptChild(Node*)
{
    return false;
}

bool DDS_WriterDataLifecycleQosPolicy::canAcceptEdge(Edge::EDGE_CLASS edgeKind, Node *dst)
{
    return false;
}
