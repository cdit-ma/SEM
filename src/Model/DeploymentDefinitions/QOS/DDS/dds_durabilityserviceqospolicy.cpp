#include "dds_durabilityserviceqospolicy.h"

#include "../../../entityfactory.h"
DDS_DurabilityServiceQosPolicy::DDS_DurabilityServiceQosPolicy(EntityFactory* factory) : Node(factory, NODE_KIND::QOS_DDS_POLICY_DURABILITYSERVICE, "DDS_DurabilityServiceQosPolicy"){
	auto node_kind = NODE_KIND::QOS_DDS_POLICY_DURABILITYSERVICE;
	QString kind_string = "DDS_DurabilityServiceQosPolicy";
	RegisterNodeKind(factory, node_kind, kind_string, [](){return new DDS_DurabilityServiceQosPolicy();});
};
DDS_DurabilityServiceQosPolicy::DDS_DurabilityServiceQosPolicy():Node(NODE_KIND::QOS_DDS_POLICY_DURABILITYSERVICE)
{
    setNodeType(NODE_TYPE::QOS); setNodeType(NODE_TYPE::DDS);

    setMoveEnabled(false);
    setExpandEnabled(false);
    updateDefaultData("label", QVariant::String, true, "durability_service");
    updateDefaultData("qos_dds_service_cleanup_delay_sec", QVariant::String, false, "0");
    updateDefaultData("qos_dds_service_cleanup_delay_nanosec", QVariant::String, false, "0");
    updateDefaultData("qos_dds_history_kind", QVariant::String, false, "KEEP_LAST_HISTORY_QOS");
    
    auto values = QStringList();
    values << "KEEP_LAST_HISTORY_QOS";
    values << "KEEP_ALL_HISTORY_QOS";
    
    updateDefaultData("qos_dds_history_depth", QVariant::String, false, "1");
    updateDefaultData("qos_dds_max_samples", QVariant::String, false, "LENGTH_UNLIMITED");
    updateDefaultData("qos_dds_max_instances", QVariant::String, false, "LENGTH_UNLIMITED");
    updateDefaultData("qos_dds_max_samples_per_instance", QVariant::String, false, "LENGTH_UNLIMITED");
}

bool DDS_DurabilityServiceQosPolicy::canAdoptChild(Node*)
{
    return false;
}

bool DDS_DurabilityServiceQosPolicy::canAcceptEdge(Edge::EDGE_KIND, Node *)
{
    return false;
}