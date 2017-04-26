#include "dds_resourcelimitsqospolicy.h"
#include "../../../data.h"

DDS_ResourceLimitsQosPolicy::DDS_ResourceLimitsQosPolicy():Node(NK_QOS_DDS_POLICY_RESOURCELIMITS)
{
    setNodeType(NT_QOS); setNodeType(NT_DDS);
}

bool DDS_ResourceLimitsQosPolicy::canAdoptChild(Node*)
{
    return false;
}

bool DDS_ResourceLimitsQosPolicy::canAcceptEdge(Edge::EDGE_KIND, Node *)
{
    return false;
}

QList<Data *> DDS_ResourceLimitsQosPolicy::getDefaultData()
{
    //QString kind = getNodeKindStr();
    auto data_list = Node::getDefaultData();
    {
        auto key = Key::GetKey("label", QVariant::String);
        auto data = new Data(key, "resource_limits");
        data->setProtected(true);
        data_list.append(data);
    }
    {
        //qos_dds_max_samples
        auto key = Key::GetKey("qos_dds_max_samples", QVariant::String);
        data_list.append(new Data(key, "LENGTH_UNLIMITED"));
    }
    {
        //qos_dds_max_instances
        auto key = Key::GetKey("qos_dds_max_instances", QVariant::String);
        data_list.append(new Data(key, "LENGTH_UNLIMITED"));
    }
    {
        //qos_dds_max_samples_per_instance
        auto key = Key::GetKey("qos_dds_max_samples_per_instance", QVariant::String);
        data_list.append(new Data(key, "LENGTH_UNLIMITED"));
    }

    return data_list;

}
