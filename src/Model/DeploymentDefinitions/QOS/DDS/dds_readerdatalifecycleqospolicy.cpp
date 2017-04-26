#include "dds_readerdatalifecycleqospolicy.h"
#include "../../../data.h"

DDS_ReaderDataLifecycleQosPolicy::DDS_ReaderDataLifecycleQosPolicy():Node(NK_QOS_DDS_POLICY_READERDATALIFECYCLE)
{
    setNodeType(NT_QOS); setNodeType(NT_DDS);
}

bool DDS_ReaderDataLifecycleQosPolicy::canAdoptChild(Node*)
{
    return false;
}

bool DDS_ReaderDataLifecycleQosPolicy::canAcceptEdge(Edge::EDGE_KIND, Node *)
{
    return false;
}

QList<Data *> DDS_ReaderDataLifecycleQosPolicy::getDefaultData()
{
    QString kind = getNodeKindStr();

    auto data_list = Node::getDefaultData();
    {
        auto key = Key::GetKey("label", QVariant::String);
        auto data = new Data(key, "reader_data_lifecycle");
        data->setProtected(true);
        data_list.append(data);
    }
    {
        auto key = Key::GetKey("qos_dds_autopurge_nowriter_samples_delay_sec", QVariant::String);
        data_list.append(new Data(key, "DURATION_INFINITE_SEC"));
    }
    {
        auto key = Key::GetKey("qos_dds_autopurge_nowriter_samples_delay_nanosec", QVariant::String);
        data_list.append(new Data(key, "DURATION_INFINITE_NSEC"));
    }
    {
        auto key = Key::GetKey("qos_dds_autopurge_disposed_samples_delay_sec", QVariant::String);
        data_list.append(new Data(key, "DURATION_INFINITE_SEC"));
    }
    {
        auto key = Key::GetKey("qos_dds_autopurge_disposed_samples_delay_nanosec", QVariant::String);
        data_list.append(new Data(key, "DURATION_INFINITE_NSEC"));
    }
    return data_list;
}
