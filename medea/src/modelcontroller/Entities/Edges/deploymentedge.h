#ifndef DEPLOYMENTEDGE_H
#define DEPLOYMENTEDGE_H
#include "../edge.h"


class EntityFactoryRegistryBroker;
class DeploymentEdge : public Edge
{
    Q_OBJECT


protected:
public:
    static void RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker);
protected:
    DeploymentEdge(EntityFactoryBroker& factory, Node *src, Node *dst);
};
#endif // AGGREGATEEDGE_H
