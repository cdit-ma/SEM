#include "dds_durabilityqospolicy.h"

#include "../../../entityfactory.h"
DDS_DurabilityQosPolicy::DDS_DurabilityQosPolicy(EntityFactory* factory) : Node(factory, NODE_KIND::QOS_DDS_POLICY_DURABILITY, "DDS_DurabilityQosPolicy"){
	auto node_kind = NODE_KIND::QOS_DDS_POLICY_DURABILITY;
	QString kind_string = "DDS_DurabilityQosPolicy";
	RegisterNodeKind(factory, node_kind, kind_string, [](){return new DDS_DurabilityQosPolicy();});
};
DDS_DurabilityQosPolicy::DDS_DurabilityQosPolicy():Node(NODE_KIND::QOS_DDS_POLICY_DURABILITY)
{
    setNodeType(NODE_TYPE::QOS); setNodeType(NODE_TYPE::DDS);

    setMoveEnabled(false);
    setExpandEnabled(false);
    updateDefaultData("label", QVariant::String, true, "durability");
    updateDefaultData("qos_dds_kind", QVariant::String, false, "VOLATILE_DURABILITY_QOS");

    auto values = QStringList();
    values << "VOLATILE_DURABILITY_QOS";
    values << "TRANSIENT_LOCAL_DURABILITY_QOS";
    values << "TRANSIENT_DURABILITY_QOS";
    values << "PERSISTENT_DURABILITY_QOS";
}

bool DDS_DurabilityQosPolicy::canAdoptChild(Node*)
{
    return false;
}

bool DDS_DurabilityQosPolicy::canAcceptEdge(Edge::EDGE_KIND, Node *)
{
    return false;
}
