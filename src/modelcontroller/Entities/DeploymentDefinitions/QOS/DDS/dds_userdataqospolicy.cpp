#include "dds_userdataqospolicy.h"
#include "../../../../entityfactory.h"

const static NODE_KIND node_kind = NODE_KIND::QOS_DDS_POLICY_USERDATA;
const static QString kind_string = "DDS_UserDataQosPolicy";

void DDS_UserDataQosPolicy::RegisterWithEntityFactory(EntityFactory& factory){
    Node::RegisterWithEntityFactory(factory, node_kind, kind_string, [](EntityFactory& factory, bool is_temp_node){
        return new DDS_UserDataQosPolicy(factory, is_temp_node);
    });
}

DDS_UserDataQosPolicy::DDS_UserDataQosPolicy(EntityFactory& factory, bool is_temp) : Node(factory, node_kind, is_temp){
    if(is_temp){
        return;
    }

    //Setup State
    setNodeType(NODE_TYPE::QOS);
    setNodeType(NODE_TYPE::DDS);

    //Setup Data
    factory.AttachData(this, "label", QVariant::String, "user_data", true);
    factory.AttachData(this, "qos_dds_str_value", QVariant::String, "", false);
}