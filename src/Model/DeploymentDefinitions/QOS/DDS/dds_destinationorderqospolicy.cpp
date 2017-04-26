#include "dds_destinationorderqospolicy.h"
#include "../../../data.h"


DDS_DestinationOrderQosPolicy::DDS_DestinationOrderQosPolicy():Node(NK_QOS_DDS_POLICY_DESTINATIONORDER)
{
    setNodeType(NT_QOS); setNodeType(NT_DDS);
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

QList<Data *> DDS_DestinationOrderQosPolicy::getDefaultData()
{
    QString kind = getNodeKindStr();
    auto data_list = Node::getDefaultData();
    {
        auto key = Key::GetKey("label", QVariant::String);
        auto data = new Data(key, "destination_order");
        data->setProtected(true);
        data_list.append(data);
    }
    {
        //qos_dds_kind
        auto key = Key::GetKey("qos_dds_kind", QVariant::String);

        if(!key->gotValidValues(kind)){
            auto values = QStringList();
            values << "BY_RECEPTION_TIMESTAMP_DESTINATIONORDER_QOS";
            values << "BY_SOURCE_TIMESTAMP_DESTINATIONORDER_QOS";
            key->addValidValues(values, QStringList(kind));
        }
        data_list.append(new Data(key, "BY_RECEPTION_TIMESTAMP_DESTINATIONORDER_QOS"));
    }
    return data_list;
}
