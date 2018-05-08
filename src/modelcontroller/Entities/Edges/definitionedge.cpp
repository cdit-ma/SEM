#include "definitionedge.h"
#include "../../edgekinds.h"

const auto edge_kind = EDGE_KIND::DEFINITION;
const QString kind_string = "Edge_Definition";

DefinitionEdge::RegisterWithEntityFactory(EntityFactory& factory, Node *src, Node *dst) : Edge(factory, src, dst, edge_kind){
    Edge::RegisterWithEntityFactory(factory, edge_kind, kind_string, [](EntityFactory& factory){return new DefinitionEdge(factory, src, dst);});
}

DefinitionEdge::DefinitionEdge(EntityFactory& factory, Node* src, Node* dst) : Edge(factory, edge_kind){
}