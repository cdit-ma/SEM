#include "dds_ownershipstrengthqospolicy.h"
#include "../../../data.h"

DDS_OwnershipStrengthQosPolicy::DDS_OwnershipStrengthQosPolicy():Node(NK_QOS_DDS_POLICY_OWNERSHIPSTRENGTH)
{
    setNodeType(NT_QOS); setNodeType(NT_DDS);
}

bool DDS_OwnershipStrengthQosPolicy::canAdoptChild(Node*)
{
    return false;
}

bool DDS_OwnershipStrengthQosPolicy::canAcceptEdge(Edge::EDGE_KIND, Node *)
{
    return false;
}

QList<Data *> DDS_OwnershipStrengthQosPolicy::getDefaultData()
{
    //QString kind = getNodeKindStr();
    auto data_list = Node::getDefaultData();
    {
        auto key = Key::GetKey("label", QVariant::String);
        auto data = new Data(key, "ownership_strength");
        data->setProtected(true);
        data_list.append(data);
    }
    {
        //qos_dds_history_kind
        auto key = Key::GetKey("qos_dds_int_value", QVariant::Int);
        data_list.append(new Data(key, 0));
    }
    return data_list;
}
