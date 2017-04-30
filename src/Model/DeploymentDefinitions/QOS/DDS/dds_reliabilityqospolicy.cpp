#include "dds_reliabilityqospolicy.h"
DDS_ReliabilityQosPolicy::DDS_ReliabilityQosPolicy():Node(NODE_KIND::QOS_DDS_POLICY_RELIABILITY)
{
    setNodeType(NODE_TYPE::QOS); setNodeType(NODE_TYPE::DDS);


    setMoveEnabled(false);
    setExpandEnabled(false);
    updateDefaultData("label", QVariant::String, true, "reliability");
    updateDefaultData("qos_dds_kind", QVariant::String, false, "RELIABLE_RELIABILITY_QOS");
    updateDefaultData("qos_dds_max_blocking_time_sec", QVariant::String, false, "0");
    updateDefaultData("qos_dds_max_blocking_time_nanosec", QVariant::String, false, "100000000");

    //qos_dds_kind
    auto values = QStringList();
    values << "BEST_EFFORT_RELIABILITY_QOS";
    values << "RELIABLE_RELIABILITY_QOS";
}

bool DDS_ReliabilityQosPolicy::canAdoptChild(Node*)
{
    return false;
}

bool DDS_ReliabilityQosPolicy::canAcceptEdge(Edge::EDGE_KIND, Node *){
    return false;
}