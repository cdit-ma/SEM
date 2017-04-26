#include "dds_topicdataqospolicy.h"
#include "../../../data.h"

DDS_TopicDataQosPolicy::DDS_TopicDataQosPolicy():Node(NK_QOS_DDS_POLICY_TOPICDATA)
{
    setNodeType(NT_QOS); setNodeType(NT_DDS);
}
bool DDS_TopicDataQosPolicy::canAdoptChild(Node*)
{
    return false;
}

bool DDS_TopicDataQosPolicy::canAcceptEdge(Edge::EDGE_KIND, Node *)
{
    return false;
}

QList<Data *> DDS_TopicDataQosPolicy::getDefaultData()
{
    QString kind = getNodeKindStr();

    auto data_list = Node::getDefaultData();
    {
        auto key = Key::GetKey("label", QVariant::String);
        auto data = new Data(key, "topic_data");
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
