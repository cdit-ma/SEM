#include "aggregateedge.h"
#include "../../entityfactory.h"

const auto edge_kind = EDGE_KIND::AGGREGATE;
const QString kind_string = "Edge_Aggregate";

void AggregateEdge::RegisterWithEntityFactory(EntityFactory& factory){
    Edge::RegisterWithEntityFactory(factory, edge_kind, kind_string, [](EntityFactory& factory, Node* src, Node* dst){
        return new AggregateEdge(factory, src, dst);
        });
}

AggregateEdge::AggregateEdge(EntityFactory& factory, Node* src, Node* dst) : Edge(factory, src, dst, edge_kind){
};