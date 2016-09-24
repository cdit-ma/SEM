#include "dds_presentationqospolicy.h"

DDS_PresentationQosPolicy::DDS_PresentationQosPolicy():Node(NK_QOS_DDS_POLICY_PRESENTATION)
{
    setNodeType(NT_QOS); setNodeType(NT_DDS);
}

bool DDS_PresentationQosPolicy::canAdoptChild(Node*)
{
    return false;
}

bool DDS_PresentationQosPolicy::canAcceptEdge(Edge::EDGE_KIND, Node *)
{
    return false;
}
