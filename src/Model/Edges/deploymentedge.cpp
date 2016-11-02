#include "deploymentedge.h"
#include "../node.h"
DeploymentEdge::DeploymentEdge(Node *src, Node *dst) : Edge(src, dst, Edge::EC_DEPLOYMENT)
{}

DeploymentEdge *DeploymentEdge::createDeploymentEdge(Node *src, Node *dst)
{
    DeploymentEdge* edge = 0;
    if(src->canAcceptEdge(Edge::EC_DEPLOYMENT, dst)){
        edge = new DeploymentEdge(src, dst);
    }
    return edge;
}
