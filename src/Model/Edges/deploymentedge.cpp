#include "deploymentedge.h"
#include "../node.h"

DeploymentEdge::DeploymentEdge(Node *src, Node *dst):Edge(src, dst, EDGE_KIND::DEPLOYMENT)
{
}

DeploymentEdge::DeploymentEdge(EntityFactory* factory):Edge(factory, EDGE_KIND::DEPLOYMENT, "Edge_Deployment"){
    auto kind = EDGE_KIND::DEPLOYMENT;
	QString kind_string = "Edge_Deployment";
	RegisterEdgeKind(factory, kind, kind_string, &DeploymentEdge::ConstructEdge);
}

DeploymentEdge *DeploymentEdge::ConstructEdge(Node *src, Node *dst)
{
    DeploymentEdge* edge = 0;
    if(src && dst){
        if(src->canAcceptEdge(EDGE_KIND::DEPLOYMENT, dst)){
            edge = new DeploymentEdge(src, dst);
        }
    }
    return edge;
}
