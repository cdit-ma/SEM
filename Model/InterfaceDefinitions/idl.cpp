#include "idl.h"

IDL::IDL(): Node(NK_IDL)
{
}

bool IDL::canAdoptChild(Node *item)
{
    switch(item->getNodeKind()){
    case NK_AGGREGATE:
    case NK_BLACKBOX:
    case NK_COMPONENT:
    case NK_VECTOR:
        break;
    default:
        return false;
    }

    //Can only adopt Aggregate and Vector's in IDL's containing Aggregates/Vectors. Otherwise all children need to match kinds.
    foreach(Node* child, getChildren(0)){
        switch(child->getNodeKind()){
        case NK_VECTOR:
        case NK_AGGREGATE:{
            if(!(item->getNodeKind() == NK_AGGREGATE || item->getNodeKind() == NK_VECTOR)){
                return false;
            }
            break;
        }
        case NK_COMPONENT:
        case NK_BLACKBOX:
            if(child->getNodeKind() != item->getNodeKind()){
                return false;
            }
            break;
        default:
            break;
        }
    }

    return Node::canAdoptChild(item);
}

bool IDL::canAcceptEdge(Edge::EDGE_KIND, Node *)
{
    return false;
}
