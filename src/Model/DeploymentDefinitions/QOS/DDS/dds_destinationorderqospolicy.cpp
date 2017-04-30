#include "dds_destinationorderqospolicy.h"

DDS_DestinationOrderQosPolicy::DDS_DestinationOrderQosPolicy():Node(NODE_KIND::QOS_DDS_POLICY_DESTINATIONORDER)
{
    setNodeType(NODE_TYPE::QOS); setNodeType(NODE_TYPE::DDS);

    setMoveEnabled(false);
    setExpandEnabled(false);
    
    updateDefaultData("label", QVariant::String, true, "destination_order");
    updateDefaultData("qos_dds_kind", QVariant::String, false, "BY_RECEPTION_TIMESTAMP_DESTINATIONORDER_QOS");
    auto values = QStringList();
    values << "BY_RECEPTION_TIMESTAMP_DESTINATIONORDER_QOS";
    values << "BY_SOURCE_TIMESTAMP_DESTINATIONORDER_QOS";
}

bool DDS_DestinationOrderQosPolicy::canAdoptChild(Node*)
{
    return false;
}

bool DDS_DestinationOrderQosPolicy::canAcceptEdge(Edge::EDGE_KIND edgeKind, Node *dst)
{
    Q_UNUSED(edgeKind);
    Q_UNUSED(dst);
    return false;
}