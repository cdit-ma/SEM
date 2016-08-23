#ifndef DEPLOYMENTDEFINITIONS_H
#define DEPLOYMENTDEFINITIONS_H
#include "../node.h"

class DeploymentDefinitions: public Node
{
    Q_OBJECT
public:
    DeploymentDefinitions();

    bool canAdoptChild(Node* child);
    bool canAcceptEdge(Edge::EDGE_CLASS edgeKind, Node *dst);
};
#endif // DEPLOYMENTDEFINITIONS_H
