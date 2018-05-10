#include "deploymentedge.h"
#include "../../entityfactorybroker.h"
#include "../../entityfactoryregistrybroker.h"

const auto edge_kind = EDGE_KIND::DEPLOYMENT;
const QString kind_string = "Edge_Deployment";

void DeploymentEdge::RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker){
    broker.RegisterWithEntityFactory(edge_kind, kind_string, [](EntityFactoryBroker& broker, Node* src, Node* dst){
        return new DeploymentEdge(broker, src, dst);
        });
}

DeploymentEdge::DeploymentEdge(EntityFactoryBroker& broker, Node* src, Node* dst) : Edge(broker, src, dst, edge_kind){
}