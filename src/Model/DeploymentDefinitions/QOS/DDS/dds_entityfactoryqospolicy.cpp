#include "dds_entityfactoryqospolicy.h"

DDS_EntityFactoryQosPolicy::DDS_EntityFactoryQosPolicy():Node(NODE_KIND::QOS_DDS_POLICY_ENTITYFACTORY)
{
    setNodeType(NODE_TYPE::QOS); setNodeType(NODE_TYPE::DDS);

    setMoveEnabled(false);
    setExpandEnabled(false);
    updateDefaultData("label", QVariant::String, true, "entity_factory");
    updateDefaultData("qos_dds_autoenable_created_entities", QVariant::Bool, false, true);
}

bool DDS_EntityFactoryQosPolicy::canAdoptChild(Node*)
{
    return false;
}

bool DDS_EntityFactoryQosPolicy::canAcceptEdge(Edge::EDGE_KIND, Node *)
{
    return false;
}