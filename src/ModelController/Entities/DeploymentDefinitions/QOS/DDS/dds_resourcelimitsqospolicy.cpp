#include "dds_resourcelimitsqospolicy.h"

DDS_ResourceLimitsQosPolicy::DDS_ResourceLimitsQosPolicy(EntityFactory* factory) : Node(factory, NODE_KIND::QOS_DDS_POLICY_RESOURCELIMITS, "DDS_ResourceLimitsQosPolicy"){
	auto node_kind = NODE_KIND::QOS_DDS_POLICY_RESOURCELIMITS;
	QString kind_string = "DDS_ResourceLimitsQosPolicy";
	RegisterNodeKind(factory, node_kind, kind_string, [](){return new DDS_ResourceLimitsQosPolicy();});

    RegisterDefaultData(factory, node_kind, "label", QVariant::String, true, "resource_limits");
    RegisterDefaultData(factory, node_kind, "qos_dds_max_samples", QVariant::String, false, "LENGTH_UNLIMITED");
    RegisterDefaultData(factory, node_kind, "qos_dds_max_instances", QVariant::String, false, "LENGTH_UNLIMITED");
    RegisterDefaultData(factory, node_kind, "qos_dds_max_samples_per_instance", QVariant::String, false, "LENGTH_UNLIMITED");
}

DDS_ResourceLimitsQosPolicy::DDS_ResourceLimitsQosPolicy():Node(NODE_KIND::QOS_DDS_POLICY_RESOURCELIMITS)
{
    setNodeType(NODE_TYPE::QOS); setNodeType(NODE_TYPE::DDS);
}

bool DDS_ResourceLimitsQosPolicy::canAdoptChild(Node*)
{
    return false;
}

bool DDS_ResourceLimitsQosPolicy::canAcceptEdge(EDGE_KIND, Node *)
{
    return false;
}
