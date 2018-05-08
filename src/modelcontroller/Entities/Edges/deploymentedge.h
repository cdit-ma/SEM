#ifndef DEPLOYMENTEDGE_H
#define DEPLOYMENTEDGE_H
#include "../edge.h"

class EntityFactory;
class DeploymentEdge : public Edge
{
    Q_OBJECT
    friend class EntityFactory;
protected:
    static void RegisterWithEntityFactory(EntityFactory& factory);
    DeploymentEdge(EntityFactory& factory, Node *src, Node *dst);
};
#endif // AGGREGATEEDGE_H
