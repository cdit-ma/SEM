#include "dds_deadlineqospolicy.h"
#include "../../../data.h"

DDS_DeadlineQosPolicy::DDS_DeadlineQosPolicy():Node(NK_QOS_DDS_POLICY_DEADLINE)
{
    setNodeType(NT_QOS); setNodeType(NT_DDS);
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

QList<Data *> DDS_DeadlineQosPolicy::getDefaultData()
{
    auto data_list = Node::getDefaultData();
    {
        auto key = Key::GetKey("label", QVariant::String);
        auto data = new Data(key, "deadline");
        data->setProtected(true);
        data_list.append(data);
    }
    {
        auto key = Key::GetKey("qos_dds_period_sec", QVariant::String);
        data_list.append(new Data(key, "DURATION_INFINITE_SEC"));
    }
    {
        auto key = Key::GetKey("qos_dds_period_nanosec", QVariant::String);
        data_list.append(new Data(key, "DURATION_INFINITE_NSEC"));
    }
    return data_list;
}
