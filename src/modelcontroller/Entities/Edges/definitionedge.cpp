#include "definitionedge.h"
#include "../../entityfactory.h"

const auto edge_kind = EDGE_KIND::DEFINITION;
const QString kind_string = "Edge_Definition";

void DefinitionEdge::RegisterWithEntityFactory(EntityFactory& factory){
    Edge::RegisterWithEntityFactory(factory, edge_kind, kind_string, [](EntityFactory& factory, Node* src, Node* dst){
        return new DefinitionEdge(factory, src, dst);
        });
}

DefinitionEdge::DefinitionEdge(EntityFactory& factory, Node* src, Node* dst) : Edge(factory, src, dst, edge_kind){
}