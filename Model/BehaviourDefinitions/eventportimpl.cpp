#include "eventportimpl.h"

EventPortImpl::EventPortImpl(Node::NODE_KIND kind):BehaviourNode(kind)
{
    setNodeType(NT_IMPLEMENTATION);
    setAcceptsEdgeKind(Edge::EC_DEFINITION);
}

bool EventPortImpl::isInPort()
{
    return getNodeKind() == NK_INEVENTPORT_IMPL;
}

bool EventPortImpl::isOutPort()
{
    return getNodeKind() == NK_OUTEVENTPORT_IMPL;
}

bool EventPortImpl::canAdoptChild(Node *child)
{
    //Can Only accept 1 child.
    if(hasChildren()){
        return false;
    }

    //Can only adopt AggregateInstances
    if(child->getNodeKind() != NK_AGGREGATE_INSTANCE){
        return false;
    }

    return BehaviourNode::canAdoptChild(child);
}

bool EventPortImpl::canAcceptEdge(Edge::EDGE_CLASS edgeKind, Node *dst)
{
    return BehaviourNode::canAcceptEdge(edgeKind, dst);
}

