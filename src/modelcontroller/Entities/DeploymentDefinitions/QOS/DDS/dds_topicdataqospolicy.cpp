#include "dds_topicdataqospolicy.h"
#include "../../../../entityfactory.h"

const static NODE_KIND node_kind = NODE_KIND::QOS_DDS_POLICY_TOPICDATA;
const static QString kind_string = "DDS_TopicDataQosPolicy";

void DDS_TopicDataQosPolicy::RegisterWithEntityFactory(EntityFactory& factory){
    Node::RegisterWithEntityFactory(factory, node_kind, kind_string, [](EntityFactory& factory, bool is_temp_node){
        return new DDS_TopicDataQosPolicy(factory, is_temp_node);
    });
}

DDS_TopicDataQosPolicy::DDS_TopicDataQosPolicy(EntityFactory& factory, bool is_temp) : Node(factory, node_kind, is_temp){
    if(is_temp){
        return;
    }

    //Setup State
    setNodeType(NODE_TYPE::QOS);
    setNodeType(NODE_TYPE::DDS);

    //Setup Data
    factory.AttachData(this, "label", QVariant::String, "topic_data", true);
    factory.AttachData(this, "qos_dds_str_value", QVariant::String, "", false);
}