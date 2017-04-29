#include "dds_deadlineqospolicy.h"

DDS_DeadlineQosPolicy::DDS_DeadlineQosPolicy():Node(NK_QOS_DDS_POLICY_DEADLINE)
{
    setNodeType(NT_QOS); setNodeType(NT_DDS);

    setMoveEnabled(false);
    setExpandEnabled(false);
    updateDefaultData("label", QVariant::String, true, "deadline");
    updateDefaultData("qos_dds_period_sec", QVariant::String, false, "DURATION_INFINITE_SEC");
    updateDefaultData("qos_dds_period_nanosec", QVariant::String, false, "DURATION_INFINITE_NSEC");
}

bool DDS_DeadlineQosPolicy::canAdoptChild(Node*)
{
    return false;
}

bool DDS_DeadlineQosPolicy::canAcceptEdge(Edge::EDGE_KIND edgeKind, Node *dst)
{
    Q_UNUSED(edgeKind);
    Q_UNUSED(dst);
    return false;
}
