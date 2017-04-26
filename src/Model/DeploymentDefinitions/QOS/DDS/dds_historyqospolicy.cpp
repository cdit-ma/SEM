#include "dds_historyqospolicy.h"
#include "../../../data.h"

DDS_HistoryQosPolicy::DDS_HistoryQosPolicy():Node(NK_QOS_DDS_POLICY_HISTORY)
{
    setNodeType(NT_QOS); setNodeType(NT_DDS);
}

bool DDS_HistoryQosPolicy::canAdoptChild(Node*)
{
    return false;
}

bool DDS_HistoryQosPolicy::canAcceptEdge(Edge::EDGE_KIND, Node *)
{
    return false;
}

QList<Data *> DDS_HistoryQosPolicy::getDefaultData()
{
    QString kind = getNodeKindStr();
    auto data_list = Node::getDefaultData();
    {
        auto key = Key::GetKey("label", QVariant::String);
        auto data = new Data(key, "history");
        data->setProtected(true);
        data_list.append(data);
    }
    {
        //qos_dds_kind
        auto key = Key::GetKey("qos_dds_kind", QVariant::String);

        if(!key->gotValidValues(kind)){
            auto values = QStringList();
            values << "KEEP_LAST_HISTORY_QOS";
            values << "KEEP_ALL_HISTORY_QOS";
            key->addValidValues(values, QStringList(kind));
        }
        data_list.append(new Data(key, "KEEP_LAST_HISTORY_QOS"));
    }
    {
        //qos_dds_depth
        auto key = Key::GetKey("qos_dds_depth", QVariant::Int);
        data_list.append(new Data(key, 1));
    }
    return data_list;
}
