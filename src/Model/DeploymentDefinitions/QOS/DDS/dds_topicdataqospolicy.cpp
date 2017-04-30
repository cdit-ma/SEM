#include "dds_topicdataqospolicy.h"

DDS_TopicDataQosPolicy::DDS_TopicDataQosPolicy():Node(NODE_KIND::QOS_DDS_POLICY_TOPICDATA)
{
    setNodeType(NODE_TYPE::QOS); setNodeType(NODE_TYPE::DDS);
    setMoveEnabled(false);
    setExpandEnabled(false);
    updateDefaultData("label", QVariant::String, true, "topic_data");
    updateDefaultData("qos_dds_str_value", QVariant::String);
}

bool DDS_TopicDataQosPolicy::canAdoptChild(Node*)
{
    return false;
}

bool DDS_TopicDataQosPolicy::canAcceptEdge(Edge::EDGE_KIND, Node *)
{
    return false;
}