#include "whileloop.h"

WhileLoop::WhileLoop():Branch(NK_WHILELOOP){
}

bool WhileLoop::canAdoptChild(Node *node)
{
    if(hasChildren()){
        return false;
    }
    return Branch::canAdoptChild(node);
}

bool WhileLoop::canAcceptEdge(Edge::EDGE_CLASS edgeKind, Node *dst)
{
    return Branch::canAcceptEdge(edgeKind, dst);
}


