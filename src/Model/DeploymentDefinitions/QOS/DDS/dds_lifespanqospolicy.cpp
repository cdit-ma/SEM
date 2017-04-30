#include "dds_lifespanqospolicy.h"

DDS_LifespanQosPolicy::DDS_LifespanQosPolicy():Node(NODE_KIND::QOS_DDS_POLICY_LIFESPAN)
{
    setNodeType(NODE_TYPE::QOS); setNodeType(NODE_TYPE::DDS);

    setMoveEnabled(false);
    setExpandEnabled(false);
    updateDefaultData("label", QVariant::String, true, "lifespan");
    updateDefaultData("qos_dds_duration_sec", QVariant::String, false, "DURATION_INFINITE_SEC");
    updateDefaultData("qos_dds_duration_nanosec", QVariant::String, false, "DURATION_INFINITE_NSEC");
}

bool DDS_LifespanQosPolicy::canAdoptChild(Node*)
{
    return false;
}

bool DDS_LifespanQosPolicy::canAcceptEdge(Edge::EDGE_KIND, Node *)
{
    return false;
}
