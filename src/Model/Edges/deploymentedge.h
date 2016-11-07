#ifndef DEPLOYMENTEDGE_H
#define DEPLOYMENTEDGE_H
#include "../edge.h"

class DeploymentEdge: public Edge{
    Q_OBJECT
protected:
    DeploymentEdge(Node* src, Node* dst);
public:
    static DeploymentEdge* createDeploymentEdge(Node* src, Node* dst);
};


#endif
