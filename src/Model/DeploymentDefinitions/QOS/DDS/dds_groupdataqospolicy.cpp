#include "dds_groupdataqospolicy.h"
#include "../../../data.h"

DDS_GroupDataQosPolicy::DDS_GroupDataQosPolicy():Node(NK_QOS_DDS_POLICY_GROUPDATA)
{
    setNodeType(NT_QOS); setNodeType(NT_DDS);
}

bool DDS_GroupDataQosPolicy::canAdoptChild(Node*)
{
    return false;
}

bool DDS_GroupDataQosPolicy::canAcceptEdge(Edge::EDGE_KIND, Node *)
{
    return false;
}

QList<Data *> DDS_GroupDataQosPolicy::getDefaultData()
{
    auto data_list = Node::getDefaultData();
    {
        auto key = Key::GetKey("label", QVariant::String);
        auto data = new Data(key, "group_data");
        data->setProtected(true);
        data_list.append(data);
    }
    {
        //qos_dds_str_value
        auto key = Key::GetKey("qos_dds_str_value", QVariant::String);
        data_list.append(new Data(key));
    }
    return data_list;
}
