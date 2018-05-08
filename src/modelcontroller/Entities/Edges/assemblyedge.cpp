#include "assemblyedge.h"
#include "../../entityfactory.h"

const auto edge_kind = EDGE_KIND::ASSEMBLY;
const QString kind_string = "Edge_Assembly";

void AssemblyEdge::RegisterWithEntityFactory(EntityFactory& factory){
    Edge::RegisterWithEntityFactory(factory, edge_kind, kind_string, [](EntityFactory& factory, Node* src, Node* dst){
        return new AssemblyEdge(factory, src, dst);
        });
}

AssemblyEdge::AssemblyEdge(EntityFactory& factory, Node* src, Node* dst) : Edge(factory, src, dst, edge_kind){
};
