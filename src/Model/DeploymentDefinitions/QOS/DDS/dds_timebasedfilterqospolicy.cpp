#include "dds_timebasedfilterqospolicy.h"
#include "../../../data.h"


DDS_TimeBasedFilterQosPolicy::DDS_TimeBasedFilterQosPolicy():Node(NK_QOS_DDS_POLICY_TIMEBASEDFILTER)
{
    setNodeType(NT_QOS); setNodeType(NT_DDS);
}

bool DDS_TimeBasedFilterQosPolicy::canAdoptChild(Node*)
{
    return false;
}

bool DDS_TimeBasedFilterQosPolicy::canAcceptEdge(Edge::EDGE_KIND, Node *)
{
    return false;
}

QList<Data *> DDS_TimeBasedFilterQosPolicy::getDefaultData()
{
    //QString kind = getNodeKindStr();
    auto data_list = Node::getDefaultData();
    {
        auto key = Key::GetKey("label", QVariant::String);
        auto data = new Data(key, "time_based_filter");
        data->setProtected(true);
        data_list.append(data);
    }
    {
        auto key = Key::GetKey("qos_dds_minimum_separation_sec", QVariant::String);
        data_list.append(new Data(key, 0));
    }
    {
        auto key = Key::GetKey("qos_dds_minimum_separation_nanosec", QVariant::String);
        data_list.append(new Data(key, 0));
    }
    return data_list;
}
