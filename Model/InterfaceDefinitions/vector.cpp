#include "vector.h"

Vector::Vector(): Node(Node::NK_VECTOR)
{
}

bool Vector::canAdoptChild(Node *child)
{
    //Can Only adopt 1x Member/AggregateInstance
    switch(child->getNodeKind()){
    case NK_MEMBER:
    case NK_AGGREGATE_INSTANCE:
        if(hasChildren()){
            return false;
        }
        break;
    default:
        return false;
    }
    return Node::canAdoptChild(child);
}

bool Vector::canAcceptEdge(Edge::EDGE_KIND edgeKind, Node *dst)
{
    if(!acceptsEdgeKind(edgeKind)){
        return false;
    }
    switch(edgeKind){
    case Edge::EC_AGGREGATE:{
        if(dst->getNodeKind() != NK_AGGREGATE){
            return false;
        }
        if(hasChildren()){
            return false;
        }
        break;
    }
    default:
        break;
    }
    return Node::canAcceptEdge(edgeKind, dst);
}
