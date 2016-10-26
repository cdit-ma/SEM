#ifndef DEPLOYMENTEDGE_H
#define DEPLOYMENTEDGE_H
#include "../edge.h"

class DeploymentEdge: public Edge{
    Q_OBJECT
public:
    //Constructor
    DeploymentEdge(Node* src, Node* dst);
    ~DeploymentEdge();
    QString toString();
};


#endif
