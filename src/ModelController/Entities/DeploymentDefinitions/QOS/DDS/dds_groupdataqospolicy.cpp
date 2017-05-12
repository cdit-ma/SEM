#include "dds_groupdataqospolicy.h"

DDS_GroupDataQosPolicy::DDS_GroupDataQosPolicy(EntityFactory* factory) : Node(factory, NODE_KIND::QOS_DDS_POLICY_GROUPDATA, "DDS_GroupDataQosPolicy"){
	auto node_kind = NODE_KIND::QOS_DDS_POLICY_GROUPDATA;
	QString kind_string = "DDS_GroupDataQosPolicy";
	RegisterNodeKind(factory, node_kind, kind_string, [](){return new DDS_GroupDataQosPolicy();});

    RegisterDefaultData(factory, node_kind, "label", QVariant::String, true, "group_data");
    RegisterDefaultData(factory, node_kind, "qos_dds_str_value", QVariant::String, false);
};

DDS_GroupDataQosPolicy::DDS_GroupDataQosPolicy():Node(NODE_KIND::QOS_DDS_POLICY_GROUPDATA)
{
    setNodeType(NODE_TYPE::QOS); setNodeType(NODE_TYPE::DDS);
}

bool DDS_GroupDataQosPolicy::canAdoptChild(Node*)
{
    return false;
}

bool DDS_GroupDataQosPolicy::canAcceptEdge(EDGE_KIND, Node *)
{
    return false;
}
