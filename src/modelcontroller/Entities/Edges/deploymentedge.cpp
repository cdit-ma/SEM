#include "deploymentedge.h"
#include "../../entityfactory.h"

const auto edge_kind = EDGE_KIND::DEPLOYMENT;
const QString kind_string = "Edge_Deployment";

void DeploymentEdge::RegisterWithEntityFactory(EntityFactory& factory){
    Edge::RegisterWithEntityFactory(factory, edge_kind, kind_string, [](EntityFactory& factory, Node* src, Node* dst){
        return new DeploymentEdge(factory, src, dst);
        });
}

DeploymentEdge::DeploymentEdge(EntityFactory& factory, Node* src, Node* dst) : Edge(factory, src, dst, edge_kind){
}