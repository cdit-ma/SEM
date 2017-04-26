#include "dds_ownershipqospolicy.h"
#include "../../../data.h"

DDS_OwnershipQosPolicy::DDS_OwnershipQosPolicy():Node(NK_QOS_DDS_POLICY_OWNERSHIP)
{
    setNodeType(NT_QOS); setNodeType(NT_DDS);
}

bool DDS_OwnershipQosPolicy::canAdoptChild(Node*)
{
    return false;
}

bool DDS_OwnershipQosPolicy::canAcceptEdge(Edge::EDGE_KIND, Node *)
{
    return false;
}

QList<Data *> DDS_OwnershipQosPolicy::getDefaultData()
{
    QString kind = getNodeKindStr();
    auto data_list = Node::getDefaultData();
    {
        auto key = Key::GetKey("label", QVariant::String);
        auto data = new Data(key, "ownership");
        data->setProtected(true);
        data_list.append(data);
    }
    {
        //qos_dds_history_kind
        auto key = Key::GetKey("qos_dds_kind", QVariant::String);

        if(!key->gotValidValues(kind)){
            auto values = QStringList();
            values << "SHARED_OWNERSHIP_QOS";
            values << "EXCLUSIVE_OWNERSHIP_QOS";
            key->addValidValues(values, QStringList(kind));
        }
        data_list.append(new Data(key, "SHARED_OWNERSHIP_QOS"));
    }
    return data_list;
}
