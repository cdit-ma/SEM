#include "dds_historyqospolicy.h"

DDS_HistoryQosPolicy::DDS_HistoryQosPolicy():Node(NK_QOS_DDS_POLICY_HISTORY)
{
    setNodeType(NT_QOS); setNodeType(NT_DDS);
}

bool DDS_HistoryQosPolicy::canAdoptChild(Node*)
{
    return false;
}

bool DDS_HistoryQosPolicy::canAcceptEdge(Edge::EDGE_KIND, Node *)
{
    return false;
}
