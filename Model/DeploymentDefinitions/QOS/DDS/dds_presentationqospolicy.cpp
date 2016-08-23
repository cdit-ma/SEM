#include "dds_presentationqospolicy.h"

DDS_PresentationQosPolicy::DDS_PresentationQosPolicy():Node(NK_QOS_DDS_POLICY_PRESENTATION)
{
    setNodeType(NT_QOS_DDS_POLICY);
}

bool DDS_PresentationQosPolicy::canAdoptChild(Node*)
{
    return false;
}

bool DDS_PresentationQosPolicy::canAcceptEdge(Edge::EDGE_CLASS edgeKind, Node *dst)
{
    return false;
}
