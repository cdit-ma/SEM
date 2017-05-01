#include "assemblyedge.h"
#include "../node.h"

AssemblyEdge::AssemblyEdge(Node *src, Node *dst):Edge(src, dst, EDGE_KIND::ASSEMBLY)
{
}

AssemblyEdge::AssemblyEdge(EntityFactory* factory):Edge(factory, EDGE_KIND::ASSEMBLY, "Edge_Assembly"){
    auto kind = EDGE_KIND::ASSEMBLY;
	QString kind_string = "Edge_Assembly";
	RegisterEdgeKind(factory, kind, kind_string, &AssemblyEdge::ConstructEdge);
}

AssemblyEdge *AssemblyEdge::ConstructEdge(Node *src, Node *dst)
{
    AssemblyEdge* edge = 0;

    if(src && dst){
        if(src->canAcceptEdge(EDGE_KIND::ASSEMBLY, dst)){
            edge = new AssemblyEdge(src, dst);
        }
    }

    return edge;
}

