#include "dds_timebasedfilterqospolicy.h"

#include "../../../entityfactory.h"
DDS_TimeBasedFilterQosPolicy::DDS_TimeBasedFilterQosPolicy(EntityFactory* factory) : Node(factory, NODE_KIND::QOS_DDS_POLICY_TIMEBASEDFILTER, "DDS_TimeBasedFilterQosPolicy"){
	auto node_kind = NODE_KIND::QOS_DDS_POLICY_TIMEBASEDFILTER;
	QString kind_string = "DDS_TimeBasedFilterQosPolicy";
	RegisterNodeKind(factory, node_kind, kind_string, [](){return new DDS_TimeBasedFilterQosPolicy();});
};
DDS_TimeBasedFilterQosPolicy::DDS_TimeBasedFilterQosPolicy():Node(NODE_KIND::QOS_DDS_POLICY_TIMEBASEDFILTER)
{
    setNodeType(NODE_TYPE::QOS); setNodeType(NODE_TYPE::DDS);

    setMoveEnabled(false);
    setExpandEnabled(false);
    updateDefaultData("label", QVariant::String, true, "time_based_filter");
    updateDefaultData("qos_dds_minimum_separation_sec", QVariant::String, false, "0");
    updateDefaultData("qos_dds_minimum_separation_nanosec", QVariant::String, false, "0");
}

bool DDS_TimeBasedFilterQosPolicy::canAdoptChild(Node*)
{
    return false;
}

bool DDS_TimeBasedFilterQosPolicy::canAcceptEdge(EDGE_KIND, Node *)
{
    return false;
}