#ifndef DEPLOYMENTDEFINITIONS_H
#define DEPLOYMENTDEFINITIONS_H
#include "../node.h"

class EntityFactory;
class DeploymentDefinitions: public Node
{
	friend class EntityFactory;
    Q_OBJECT
protected:
	DeploymentDefinitions(EntityFactory* factory);
	DeploymentDefinitions();
public:
    bool canAdoptChild(Node* child);
    bool canAcceptEdge(Edge::EDGE_KIND edgeKind, Node *dst);
};
#endif // DEPLOYMENTDEFINITIONS_H
