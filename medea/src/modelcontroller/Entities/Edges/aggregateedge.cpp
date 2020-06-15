#include "aggregateedge.h"
#include "../../entityfactorybroker.h"
#include "../../entityfactoryregistrybroker.h"

const auto edge_kind = EDGE_KIND::AGGREGATE;
const QString kind_string = "Edge_Aggregate";

void AggregateEdge::RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker){
    broker.RegisterWithEntityFactory(edge_kind, kind_string, [](EntityFactoryBroker& broker, Node* src, Node* dst){
        return new AggregateEdge(broker, src, dst);
        });
}

AggregateEdge::AggregateEdge(EntityFactoryBroker& broker, Node* src, Node* dst) : Edge(broker, src, dst, edge_kind){
};