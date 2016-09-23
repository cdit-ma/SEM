#include "dds_topicdataqospolicy.h"

DDS_TopicDataQosPolicy::DDS_TopicDataQosPolicy():Node(NK_QOS_DDS_POLICY_TOPICDATA)
{
    setNodeType(NT_QOS); setNodeType(NT_DDS);
}
bool DDS_TopicDataQosPolicy::canAdoptChild(Node*)
{
    return false;
}

bool DDS_TopicDataQosPolicy::canAcceptEdge(Edge::EDGE_KIND, Node *)
{
    return false;
}
