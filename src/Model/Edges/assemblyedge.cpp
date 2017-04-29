#include "assemblyedge.h"
#include "../node.h"

AssemblyEdge::AssemblyEdge(Node *src, Node *dst):Edge(src, dst, Edge::EC_ASSEMBLY)
{
}

AssemblyEdge *AssemblyEdge::createAssemblyEdge(Node *src, Node *dst)
{
    AssemblyEdge* edge = 0;

    if(src && dst){
        if(src->canAcceptEdge(Edge::EC_ASSEMBLY, dst)){
            edge = new AssemblyEdge(src, dst);
        }
    }else if(!src && !dst){
        //Allow an empty edge
        edge = new AssemblyEdge(0, 0);
    }

    return edge;
}

