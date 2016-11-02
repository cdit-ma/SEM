#include "assemblyedge.h"
#include "../node.h"

AssemblyEdge::AssemblyEdge(Node *src, Node *dst):Edge(src, dst, Edge::EC_ASSEMBLY)
{
}

AssemblyEdge *AssemblyEdge::createAssemblyEdge(Node *src, Node *dst)
{
    AssemblyEdge* edge = 0;
    if(src->canAcceptEdge(Edge::EC_ASSEMBLY, dst)){
        edge = new AssemblyEdge(src, dst);
    }
    return edge;

}

