#include "dataedge.h"
#include "../node.h"
#include "../../entityfactorybroker.h"
#include "../../entityfactoryregistrybroker.h"

const static EDGE_KIND edge_kind = EDGE_KIND::DATA;
const static QString kind_string = "Edge_Data";

void DataEdge::RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker){
    broker.RegisterWithEntityFactory(edge_kind, kind_string, [](EntityFactoryBroker& broker, Node* src, Node* dst){
        return new DataEdge(broker, src, dst);
        });
}

DataEdge::DataEdge(EntityFactoryBroker& broker, Node* src, Node* dst) : Edge(broker, src, dst, edge_kind){
};