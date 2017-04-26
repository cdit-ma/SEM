#include "dds_durabilityqospolicy.h"
#include "../../../data.h"

DDS_DurabilityQosPolicy::DDS_DurabilityQosPolicy():Node(NK_QOS_DDS_POLICY_DURABILITY)
{
    setNodeType(NT_QOS); setNodeType(NT_DDS);
}

bool DDS_DurabilityQosPolicy::canAdoptChild(Node*)
{
    return false;
}

bool DDS_DurabilityQosPolicy::canAcceptEdge(Edge::EDGE_KIND, Node *)
{
    return false;
}

QList<Data *> DDS_DurabilityQosPolicy::getDefaultData()
{
    QString kind = getNodeKindStr();
    auto data_list = Node::getDefaultData();
    {
        auto key = Key::GetKey("label", QVariant::String);
        auto data = new Data(key, "durability");
        data->setProtected(true);
        data_list.append(data);
    }
    {
        //qos_dds_kind
        auto key = Key::GetKey("qos_dds_kind", QVariant::String);

        if(!key->gotValidValues(kind)){
            auto values = QStringList();
            values << "VOLATILE_DURABILITY_QOS";
            values << "TRANSIENT_LOCAL_DURABILITY_QOS";
            values << "TRANSIENT_DURABILITY_QOS";
            values << "PERSISTENT_DURABILITY_QOS";
            key->addValidValues(values, QStringList(kind));
        }
        data_list.append(new Data(key, "VOLATILE_DURABILITY_QOS"));
    }
    return data_list;
}
