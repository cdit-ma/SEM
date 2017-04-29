#include "dds_durabilityqospolicy.h"

DDS_DurabilityQosPolicy::DDS_DurabilityQosPolicy():Node(NK_QOS_DDS_POLICY_DURABILITY)
{
    setNodeType(NT_QOS); setNodeType(NT_DDS);

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
