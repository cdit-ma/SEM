#include "dds_livelinessqospolicy.h"

DDS_LivelinessQosPolicy::DDS_LivelinessQosPolicy():Node(NK_QOS_DDS_POLICY_LIVELINESS)
{
    setNodeType(NT_QOS); setNodeType(NT_DDS);
}

bool DDS_LivelinessQosPolicy::canAdoptChild(Node*)
{
    return false;
}

bool DDS_LivelinessQosPolicy::canAcceptEdge(Edge::EDGE_KIND, Node *)
{
    return false;
}
