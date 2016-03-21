#include "deploymentedge.h"
#include "../node.h"
DeploymentEdge::DeploymentEdge(Node *src, Node *dst) : Edge(src, dst, Edge::EC_DEPLOYMENT)
{}

DeploymentEdge::~DeploymentEdge()
{
}


QString DeploymentEdge::toString()
{
    return QString("DeploymentEdge[%1]: [" + getSource()->toString() +"] <-> [" + getDestination()->toString() + "]").arg(this->getID());
}
