#include "dds_topicdataqospolicy.h"

#include "../../../entityfactory.h"
DDS_TopicDataQosPolicy::DDS_TopicDataQosPolicy(EntityFactory* factory) : Node(factory, NODE_KIND::QOS_DDS_POLICY_TOPICDATA, "DDS_TopicDataQosPolicy"){
	auto node_kind = NODE_KIND::QOS_DDS_POLICY_TOPICDATA;
	QString kind_string = "DDS_TopicDataQosPolicy";
	RegisterNodeKind(factory, node_kind, kind_string, [](){return new DDS_TopicDataQosPolicy();});
};
DDS_TopicDataQosPolicy::DDS_TopicDataQosPolicy():Node(NODE_KIND::QOS_DDS_POLICY_TOPICDATA)
{
    setNodeType(NODE_TYPE::QOS); setNodeType(NODE_TYPE::DDS);
    //setMoveEnabled(false);
    //setExpandEnabled(false);
    //updateDefaultData("label", QVariant::String, true, "topic_data");
    //updateDefaultData("qos_dds_str_value", QVariant::String);
}

bool DDS_TopicDataQosPolicy::canAdoptChild(Node*)
{
    return false;
}

bool DDS_TopicDataQosPolicy::canAcceptEdge(EDGE_KIND, Node *)
{
    return false;
}