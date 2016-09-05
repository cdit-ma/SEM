#include "blackboxinstance.h"

BlackBoxInstance::BlackBoxInstance():Node(Node::NK_BLACKBOX_INSTANCE)
{
    setNodeType(NT_INSTANCE);
    setAcceptsEdgeKind(Edge::EC_DEFINITION);
    setAcceptsEdgeKind(Edge::EC_DEPLOYMENT);
}

bool BlackBoxInstance::canAdoptChild(Node *child)
{
    //Can Only adopt EventPort Instances
    switch(child->getNodeKind()){
    case NK_INEVENTPORT_INSTANCE:
    case NK_OUTEVENTPORT_INSTANCE:
        break;
    default:
        return false;
    }

    return Node::canAdoptChild(child);
}

bool BlackBoxInstance::canAcceptEdge(Edge::EDGE_KIND edgeKind, Node *dst)
{
    if(!acceptsEdgeKind(edgeKind)){
        return false;
    }
    switch(edgeKind){
    case Edge::EC_DEFINITION:{
        if(dst->getNodeKind() != NK_BLACKBOX){
            return false;
        }
        break;
    }
    default:
        break;
    }
    return Node::canAcceptEdge(edgeKind, dst);
}
