#include "dds_groupdataqospolicy.h"
#include "../../../../entityfactory.h"

const static NODE_KIND node_kind = NODE_KIND::QOS_DDS_POLICY_GROUPDATA;
const static QString kind_string = "DDS_GroupDataQosPolicy";

void DDS_GroupDataQosPolicy::RegisterWithEntityFactory(EntityFactory& factory){
    Node::RegisterWithEntityFactory(factory, node_kind, kind_string, [](EntityFactory& factory, bool is_temp_node){
        return new DDS_GroupDataQosPolicy(factory, is_temp_node);
    });
}

DDS_GroupDataQosPolicy::DDS_GroupDataQosPolicy(EntityFactory& factory, bool is_temp) : Node(factory, node_kind, is_temp){
    if(is_temp){
        return;
    }

    //Setup State
    setNodeType(NODE_TYPE::QOS);
    setNodeType(NODE_TYPE::DDS);

    //Setup Data
    factory.AttachData(this, "label", QVariant::String, "group_data", true);
    factory.AttachData(this, "qos_dds_str_value", QVariant::String, "", false);
}