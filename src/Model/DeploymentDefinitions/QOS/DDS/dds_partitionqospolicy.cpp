#include "dds_partitionqospolicy.h"
#include "../../../data.h"

DDS_PartitionQosPolicy::DDS_PartitionQosPolicy():Node(NK_QOS_DDS_POLICY_PARTITION)
{
    setNodeType(NT_QOS); setNodeType(NT_DDS);
}

bool DDS_PartitionQosPolicy::canAdoptChild(Node*)
{
    return false;
}

bool DDS_PartitionQosPolicy::canAcceptEdge(Edge::EDGE_KIND, Node *)
{
    return false;
}

QList<Data *> DDS_PartitionQosPolicy::getDefaultData()
{
    //QString kind = getNodeKindStr();
    auto data_list = Node::getDefaultData();
    {
        auto key = Key::GetKey("label", QVariant::String);
        auto data = new Data(key, "partition");
        data->setProtected(true);
        data_list.append(data);
    }
    {
        auto key = Key::GetKey("qos_dds_name", QVariant::String);
        data_list.append(new Data(key));
    }
    return data_list;
}
