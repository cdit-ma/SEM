#include "dds_presentationqospolicy.h"
#include "../../../data.h"

DDS_PresentationQosPolicy::DDS_PresentationQosPolicy():Node(NK_QOS_DDS_POLICY_PRESENTATION)
{
    setNodeType(NT_QOS); setNodeType(NT_DDS);
}

bool DDS_PresentationQosPolicy::canAdoptChild(Node*)
{
    return false;
}

bool DDS_PresentationQosPolicy::canAcceptEdge(Edge::EDGE_KIND, Node *)
{
    return false;
}

QList<Data *> DDS_PresentationQosPolicy::getDefaultData()
{
    QString kind = getNodeKindStr();
    auto data_list = Node::getDefaultData();
    {
        auto key = Key::GetKey("label", QVariant::String);
        auto data = new Data(key, "presentation");
        data->setProtected(true);
        data_list.append(data);
    }
    {
        //qos_dds_history_kind
        auto key = Key::GetKey("qos_dds_access_scope", QVariant::String);
        if(!key->gotValidValues(kind)){
            auto values = QStringList();
            values << "INSTANCE_PRESENTATION_QOS";
            values << "TOPIC_PRESENTATION_QOS";
            values << "GROUP_PRESENTATION_QOS";
            key->addValidValues(values, QStringList(kind));
        }
        data_list.append(new Data(key, "INSTANCE_PRESENTATION_QOS"));
    }
    {
        auto key = Key::GetKey("qos_dds_coherent_access", QVariant::Bool);
        data_list.append(new Data(key, false));
    }
    {
        auto key = Key::GetKey("qos_dds_ordered_access", QVariant::Bool);
        data_list.append(new Data(key, false));
    }

    return data_list;
}
