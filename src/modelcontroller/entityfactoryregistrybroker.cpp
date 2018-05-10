#include "entityfactoryregistrybroker.h"
#include "entityfactorybroker.h"
#include "entityfactory.h"

EntityFactoryRegistryBroker::EntityFactoryRegistryBroker(EntityFactory& factory) : factory_(factory){

}

void EntityFactoryRegistryBroker::RegisterWithEntityFactory(const NODE_KIND& node_kind, const QString& kind_string, std::function<Node* (EntityFactoryBroker&, bool)> constructor){
    factory_.RegisterNodeKind(node_kind, kind_string, constructor);
}

void EntityFactoryRegistryBroker::RegisterWithEntityFactory(const EDGE_KIND& edge_kind, const QString& kind_string, std::function<Edge* (EntityFactoryBroker&, Node*, Node*)> constructor){
    factory_.RegisterEdgeKind(edge_kind, kind_string, constructor);
}
