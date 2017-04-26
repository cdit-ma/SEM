#include "dds_transportpriorityqospolicy.h"
#include "../../../data.h"

DDS_TransportPriorityQosPolicy::DDS_TransportPriorityQosPolicy():Node(NK_QOS_DDS_POLICY_TRANSPORTPRIORITY)
{
    setNodeType(NT_QOS); setNodeType(NT_DDS);
}

bool DDS_TransportPriorityQosPolicy::canAdoptChild(Node*)
{
    return false;
}

bool DDS_TransportPriorityQosPolicy::canAcceptEdge(Edge::EDGE_KIND, Node *)
{
    return false;
}

QList<Data *> DDS_TransportPriorityQosPolicy::getDefaultData()
{
    QString kind = getNodeKindStr();

    auto data_list = Node::getDefaultData();
    {
        auto key = Key::GetKey("label", QVariant::String);
        auto data = new Data(key, "transport_priority");
        data->setProtected(true);
        data_list.append(data);
    }
    {
        //qos_dds_str_value
        auto key = Key::GetKey("qos_dds_int_value", QVariant::Int);
        data_list.append(new Data(key, 0));
    }

    return data_list;
}
