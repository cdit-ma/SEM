#include "dds_lifespanqospolicy.h"
#include "../../../data.h"

DDS_LifespanQosPolicy::DDS_LifespanQosPolicy():Node(NK_QOS_DDS_POLICY_LIFESPAN)
{
    setNodeType(NT_QOS); setNodeType(NT_DDS);
}

bool DDS_LifespanQosPolicy::canAdoptChild(Node*)
{
    return false;
}

bool DDS_LifespanQosPolicy::canAcceptEdge(Edge::EDGE_KIND, Node *)
{
    return false;
}

QList<Data *> DDS_LifespanQosPolicy::getDefaultData()
{
    auto data_list = Node::getDefaultData();
    {
        auto key = Key::GetKey("label", QVariant::String);
        auto data = new Data(key, "lifespan");
        data->setProtected(true);
        data_list.append(data);
    }
    {
        auto key = Key::GetKey("qos_dds_duration_sec", QVariant::String);
        data_list.append(new Data(key, "DURATION_INFINITE_SEC"));
    }
    {
        auto key = Key::GetKey("qos_dds_duration_nanosec", QVariant::String);
        data_list.append(new Data(key, "DURATION_INFINITE_NSEC"));
    }

    return data_list;
}
