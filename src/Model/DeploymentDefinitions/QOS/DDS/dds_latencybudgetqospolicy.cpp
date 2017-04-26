#include "dds_latencybudgetqospolicy.h"
#include "../../../data.h"

DDS_LatencyBudgetQosPolicy::DDS_LatencyBudgetQosPolicy():Node(NK_QOS_DDS_POLICY_LATENCYBUDGET)
{
    setNodeType(NT_QOS); setNodeType(NT_DDS);
}

bool DDS_LatencyBudgetQosPolicy::canAdoptChild(Node*)
{
    return false;
}

bool DDS_LatencyBudgetQosPolicy::canAcceptEdge(Edge::EDGE_KIND, Node *)
{
    return false;
}

QList<Data *> DDS_LatencyBudgetQosPolicy::getDefaultData()
{
    auto data_list = Node::getDefaultData();
    {
        auto key = Key::GetKey("label", QVariant::String);
        auto data = new Data(key, "latency_budget");
        data->setProtected(true);
        data_list.append(data);
    }
    {
        //qos_dds_service_cleanup_delay_sec
        auto key = Key::GetKey("qos_dds_duration_sec", QVariant::String);
        data_list.append(new Data(key, 0));
    }
    {
        //qos_dds_service_cleanup_delay_nanosec
        auto key = Key::GetKey("qos_dds_duration_nanosec", QVariant::String);
        data_list.append(new Data(key, 0));
    }
    return data_list;
}
