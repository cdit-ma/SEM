#include "dds_reliabilityqospolicy.h"
#include "../../../data.h"

DDS_ReliabilityQosPolicy::DDS_ReliabilityQosPolicy():Node(NK_QOS_DDS_POLICY_RELIABILITY)
{
    setNodeType(NT_QOS); setNodeType(NT_DDS);
}

bool DDS_ReliabilityQosPolicy::canAdoptChild(Node*)
{
    return false;
}

bool DDS_ReliabilityQosPolicy::canAcceptEdge(Edge::EDGE_KIND, Node *)
{
    return false;
}

QList<Data *> DDS_ReliabilityQosPolicy::getDefaultData()
{
    QString kind = getNodeKindStr();

    auto data_list = Node::getDefaultData();
    {
        auto key = Key::GetKey("label", QVariant::String);
        auto data = new Data(key, "reliability");
        data->setProtected(true);
        data_list.append(data);
    }
    {
        //qos_dds_history_kind
        auto key = Key::GetKey("qos_dds_kind", QVariant::String);

        if(!key->gotValidValues(kind)){
            auto values = QStringList();
            values << "BEST_EFFORT_RELIABILITY_QOS";
            values << "RELIABLE_RELIABILITY_QOS";
            key->addValidValues(values, QStringList(kind));
        }
        data_list.append(new Data(key, "RELIABLE_RELIABILITY_QOS"));
    }
    {
        auto key = Key::GetKey("qos_dds_max_blocking_time_sec", QVariant::String);
        data_list.append(new Data(key, 0));
    }
    {
        auto key = Key::GetKey("qos_dds_max_blocking_time_nanosec", QVariant::String);
        data_list.append(new Data(key, 100000000));
    }
    return data_list;
}
