#ifndef DEPLOYMENTEDGE_H
#define DEPLOYMENTEDGE_H
#include "../edge.h"

class EntityFactory;
class DeploymentEdge : public Edge
{
    Q_OBJECT
    friend class EntityFactory;
protected:
    DeploymentEdge(EntityFactory* factory);
    DeploymentEdge(Node *src, Node *dst);
private:
    static DeploymentEdge* ConstructEdge(Node* src, Node* dst);
};
#endif // AGGREGATEEDGE_H
