#include "dds_entityfactoryqospolicy.h"

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
