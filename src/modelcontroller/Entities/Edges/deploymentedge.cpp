#include "deploymentedge.h"
#include "../../edgekinds.h"

const auto edge_kind = EDGE_KIND::DEPLOYMENT;
const QString kind_string = "Edge_Deployment";

DeploymentEdge::RegisterWithEntityFactory(EntityFactory& factory, Node *src, Node *dst) : Edge(factory, src, dst, edge_kind){
    Edge::RegisterWithEntityFactory(factory, edge_kind, kind_string, [](EntityFactory& factory){return new DeploymentEdge(factory, src, dst);});
}

DeploymentEdge::DeploymentEdge(EntityFactory& factory, Node* src, Node* dst) : Edge(factory, edge_kind){
}