#include "dds_userdataqospolicy.h"
#include "../../../data.h"

DDS_UserDataQosPolicy::DDS_UserDataQosPolicy():Node(NK_QOS_DDS_POLICY_USERDATA)
{
    setNodeType(NT_QOS); setNodeType(NT_DDS);
}


bool DDS_UserDataQosPolicy::canAdoptChild(Node*)
{
    return false;
}

bool DDS_UserDataQosPolicy::canAcceptEdge(Edge::EDGE_KIND, Node *)
{
    return false;
}

QList<Data *> DDS_UserDataQosPolicy::getDefaultData()
{
    auto data_list = Node::getDefaultData();
    {
        auto key = Key::GetKey("label", QVariant::String);
        auto data = new Data(key, "user_data");
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
