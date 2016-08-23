#include "dds_entityfactoryqospolicy.h"

DDS_EntityFactoryQosPolicy::DDS_EntityFactoryQosPolicy():Node(NK_QOS_DDS_POLICY_ENTITYFACTORY)
{
    setNodeType(NT_QOS_DDS_POLICY);
}

bool DDS_EntityFactoryQosPolicy::canAdoptChild(Node*)
{
    return false;
}

bool DDS_EntityFactoryQosPolicy::canAcceptEdge(Edge::EDGE_CLASS edgeKind, Node *dst)
{
    return false;
}
