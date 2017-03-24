#include "whileloop.h"

WhileLoop::WhileLoop():Branch(NK_WHILELOOP){
}

bool WhileLoop::canAdoptChild(Node *node)
{
    //Only allow one type.
    if(hasChildren()){
        return false;
    }
    return Branch::canAdoptChild(node);
}

bool WhileLoop::canAcceptEdge(Edge::EDGE_KIND edgeKind, Node *dst)
{
    return Branch::canAcceptEdge(edgeKind, dst);
}


