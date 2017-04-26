#include "dds_entityfactoryqospolicy.h"
#include "../../../data.h"

DDS_EntityFactoryQosPolicy::DDS_EntityFactoryQosPolicy():Node(NK_QOS_DDS_POLICY_ENTITYFACTORY)
{
    setNodeType(NT_QOS); setNodeType(NT_DDS);
}

bool DDS_EntityFactoryQosPolicy::canAdoptChild(Node*)
{
    return false;
}

bool DDS_EntityFactoryQosPolicy::canAcceptEdge(Edge::EDGE_KIND, Node *)
{
    return false;
}

QList<Data *> DDS_EntityFactoryQosPolicy::getDefaultData()
{
    auto data_list = Node::getDefaultData();
    {
        auto key = Key::GetKey("label", QVariant::String);
        auto data = new Data(key, "entity_factory");
        data->setProtected(true);
        data_list.append(data);
    }
    {
        //qos_dds_autoenable_created_entities
        auto key = Key::GetKey("qos_dds_autoenable_created_entities", QVariant::Bool);
        data_list.append(new Data(key, true));
    }
    return data_list;
}
