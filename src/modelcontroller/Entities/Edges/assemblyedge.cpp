#include "assemblyedge.h"
#include "../../edgekinds.h"

const auto edge_kind = EDGE_KIND::ASSEMBLY;
const QString kind_string = "Edge_Assembly";

AssemblyEdge::RegisterWithEntityFactory(EntityFactory& factory, Node *src, Node *dst) : Edge(factory, src, dst, edge_kind){
    Edge::RegisterWithEntityFactory(factory, edge_kind, kind_string, [](EntityFactory& factory){return new AssemblyEdge(factory, src, dst);});
}

AssemblyEdge::AssemblyEdge(EntityFactory& factory, Node* src, Node* dst) : Edge(factory, edge_kind){
};
