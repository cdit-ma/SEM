#include "componentinstance.h"

ComponentInstance::ComponentInstance():Node(Node::NK_COMPONENT_INSTANCE)
{
    setNodeType(NT_INSTANCE);
    setAcceptsEdgeKind(Edge::EC_DEFINITION);
    setAcceptsEdgeKind(Edge::EC_DEPLOYMENT);
    setAcceptsEdgeKind(Edge::EC_QOS);
}

bool ComponentInstance::canAdoptChild(Node *child)
{

    switch(child->getNodeKind()){
    case NK_ATTRIBUTE_INSTANCE:
    case NK_INEVENTPORT_INSTANCE:
    case NK_OUTEVENTPORT_INSTANCE:
        break;
    default:
        return false;
    }
    return Node::canAdoptChild(child);
}

bool ComponentInstance::canAcceptEdge(Edge::EDGE_CLASS edgeKind, Node *dst)
{
    if(!acceptsEdgeKind(edgeKind)){
        return false;
    }

    switch(edgeKind){
    case Edge::EC_DEFINITION:{
        if(dst->getNodeKind() != NK_COMPONENT){
            return false;
        }
        break;
    }
    default:
        break;
    }
    return Node::canAcceptEdge(edgeKind, dst);

}
