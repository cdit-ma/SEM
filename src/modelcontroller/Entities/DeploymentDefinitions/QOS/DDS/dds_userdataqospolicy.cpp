#include "dds_userdataqospolicy.h"

DDS_UserDataQosPolicy::DDS_UserDataQosPolicy(EntityFactory* factory) : Node(factory, NODE_KIND::QOS_DDS_POLICY_USERDATA, "DDS_UserDataQosPolicy"){
	auto node_kind = NODE_KIND::QOS_DDS_POLICY_USERDATA;
	QString kind_string = "DDS_UserDataQosPolicy";
	RegisterNodeKind(factory, node_kind, kind_string, [](){return new DDS_UserDataQosPolicy();});

    RegisterDefaultData(factory, node_kind, "label", QVariant::String, true, "user_data");
    RegisterDefaultData(factory, node_kind, "qos_dds_str_value", QVariant::String, false);
}

DDS_UserDataQosPolicy::DDS_UserDataQosPolicy():Node(NODE_KIND::QOS_DDS_POLICY_USERDATA)
{
    setNodeType(NODE_TYPE::QOS); setNodeType(NODE_TYPE::DDS);
}