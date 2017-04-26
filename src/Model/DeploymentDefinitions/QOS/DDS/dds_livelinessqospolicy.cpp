#include "dds_livelinessqospolicy.h"
#include "../../../data.h"

DDS_LivelinessQosPolicy::DDS_LivelinessQosPolicy():Node(NK_QOS_DDS_POLICY_LIVELINESS)
{
    setNodeType(NT_QOS); setNodeType(NT_DDS);
}

bool DDS_LivelinessQosPolicy::canAdoptChild(Node*)
{
    return false;
}

bool DDS_LivelinessQosPolicy::canAcceptEdge(Edge::EDGE_KIND, Node *)
{
    return false;
}

QList<Data *> DDS_LivelinessQosPolicy::getDefaultData()
{
    QString kind = getNodeKindStr();
    auto data_list = Node::getDefaultData();
    {
        auto key = Key::GetKey("label", QVariant::String);
        auto data = new Data(key, "liveliness");
        data->setProtected(true);
        data_list.append(data);
    }
    {
        //qos_dds_history_kind
        auto key = Key::GetKey("qos_dds_kind", QVariant::String);

        if(!key->gotValidValues(kind)){
            auto values = QStringList();
            values << "AUTOMATIC_LIVELINESS_QOS";
            values << "MANUAL_BY_PARTICIPANT_LIVELINESS_QOS";
            values << "MANUAL_BY_TOPIC_LIVELINESS_QOS";
            key->addValidValues(values, QStringList(kind));
        }
        data_list.append(new Data(key, "AUTOMATIC_LIVELINESS_QOS"));
    }
    {
        //qos_dds_lease_duration_sec
        auto key = Key::GetKey("qos_dds_lease_duration_sec", QVariant::String);
        data_list.append(new Data(key, "DURATION_INFINITE_SEC"));
    }
    {
        //qos_dds_lease_duration_nanosec
        auto key = Key::GetKey("qos_dds_lease_duration_nanosec", QVariant::String);
        data_list.append(new Data(key, "DURATION_INFINITE_NSEC"));
    }
    return data_list;

}
