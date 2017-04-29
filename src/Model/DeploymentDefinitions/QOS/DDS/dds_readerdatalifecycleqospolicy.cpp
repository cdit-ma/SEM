#include "dds_readerdatalifecycleqospolicy.h"

DDS_ReaderDataLifecycleQosPolicy::DDS_ReaderDataLifecycleQosPolicy():Node(NK_QOS_DDS_POLICY_READERDATALIFECYCLE)
{
    setNodeType(NT_QOS); setNodeType(NT_DDS);

    setMoveEnabled(false);
    setExpandEnabled(false);
    updateDefaultData("label", QVariant::String, true, "reader_data_lifecycle");
    updateDefaultData("qos_dds_autopurge_nowriter_samples_delay_sec", QVariant::String, false, "DURATION_INFINITE_SEC");
    updateDefaultData("qos_dds_autopurge_nowriter_samples_delay_nanosec", QVariant::String, false, "DURATION_INFINITE_NSEC");
    updateDefaultData("qos_dds_autopurge_disposed_samples_delay_sec", QVariant::String, false, "DURATION_INFINITE_SEC");
    updateDefaultData("qos_dds_autopurge_disposed_samples_delay_nanosec", QVariant::String, false, "DURATION_INFINITE_NSEC");
}

bool DDS_ReaderDataLifecycleQosPolicy::canAdoptChild(Node*)
{
    return false;
}

bool DDS_ReaderDataLifecycleQosPolicy::canAcceptEdge(Edge::EDGE_KIND, Node *)
{
    return false;
}