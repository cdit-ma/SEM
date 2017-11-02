#include "dds_destinationorderqospolicy.h"

DDS_DestinationOrderQosPolicy::DDS_DestinationOrderQosPolicy(EntityFactory* factory) : Node(factory, NODE_KIND::QOS_DDS_POLICY_DESTINATIONORDER, "DDS_DestinationOrderQosPolicy"){
	auto node_kind = NODE_KIND::QOS_DDS_POLICY_DESTINATIONORDER;
	QString kind_string = "DDS_DestinationOrderQosPolicy";
	RegisterNodeKind(factory, node_kind, kind_string, [](){return new DDS_DestinationOrderQosPolicy();});

    QList<QVariant> values;
    values << "BY_RECEPTION_TIMESTAMP_DESTINATIONORDER_QOS";
    values << "BY_SOURCE_TIMESTAMP_DESTINATIONORDER_QOS";

    RegisterValidDataValues(factory, node_kind, "qos_dds_kind", QVariant::String, values);

    RegisterDefaultData(factory, node_kind, "label", QVariant::String, true, "destination_order");
    RegisterDefaultData(factory, node_kind, "qos_dds_kind", QVariant::String, false, values.first());
};

DDS_DestinationOrderQosPolicy::DDS_DestinationOrderQosPolicy():Node(NODE_KIND::QOS_DDS_POLICY_DESTINATIONORDER)
{
    setNodeType(NODE_TYPE::QOS); setNodeType(NODE_TYPE::DDS);
}

bool DDS_DestinationOrderQosPolicy::canAdoptChild(Node*)
{
    return false;
}

bool DDS_DestinationOrderQosPolicy::canAcceptEdge(EDGE_KIND edgeKind, Node *dst)
{
    Q_UNUSED(edgeKind);
    Q_UNUSED(dst);
    return false;
}