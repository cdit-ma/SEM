#include "aggregateedge.h"
#include "../../edgekinds.h"

const auto edge_kind = EDGE_KIND::AGGREGATE;
const QString kind_string = "Edge_Aggregate";

AggregateEdge::RegisterWithEntityFactory(EntityFactory& factory, Node *src, Node *dst) : Edge(factory, src, dst, edge_kind){
    Edge::RegisterWithEntityFactory(factory, edge_kind, kind_string, [](EntityFactory& factory){return new AggregateEdge(factory, src, dst);});
}

AggregateEdge::AggregateEdge(EntityFactory& factory, Node* src, Node* dst) : Edge(factory, edge_kind){
};