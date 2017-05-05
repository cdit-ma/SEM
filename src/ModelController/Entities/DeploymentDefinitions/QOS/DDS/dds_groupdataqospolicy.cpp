#include "dds_groupdataqospolicy.h"



DDS_GroupDataQosPolicy::DDS_GroupDataQosPolicy(EntityFactory* factory) : Node(factory, NODE_KIND::QOS_DDS_POLICY_GROUPDATA, "DDS_GroupDataQosPolicy"){
	auto node_kind = NODE_KIND::QOS_DDS_POLICY_GROUPDATA;
	QString kind_string = "DDS_GroupDataQosPolicy";
	RegisterNodeKind(factory, node_kind, kind_string, [](){return new DDS_GroupDataQosPolicy();});
};
DDS_GroupDataQosPolicy::DDS_GroupDataQosPolicy():Node(NODE_KIND::QOS_DDS_POLICY_GROUPDATA)
{
    setNodeType(NODE_TYPE::QOS); setNodeType(NODE_TYPE::DDS);

    //setMoveEnabled(false);
    //setExpandEnabled(false);
    //updateDefaultData("label", QVariant::String, true, "group_data");
    //updateDefaultData("qos_dds_str_value", QVariant::String, false);
}

bool DDS_GroupDataQosPolicy::canAdoptChild(Node*)
{
    return false;
}

bool DDS_GroupDataQosPolicy::canAcceptEdge(EDGE_KIND, Node *)
{
    return false;
}
