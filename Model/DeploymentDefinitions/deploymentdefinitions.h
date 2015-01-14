#ifndef DEPLOYMENTDEFINITIONS_H
#define DEPLOYMENTDEFINITIONS_H

#include "../node.h"

class DeploymentDefinitions: public Node
{
    Q_OBJECT
public:
    DeploymentDefinitions();
    ~DeploymentDefinitions();


    // GraphML interface
public:


    // Node interface
public:
    bool canConnect(Node* attachableObject);
    bool canAdoptChild(Node* child);
};
#endif // DEPLOYMENTDEFINITIONS_H
