#include "eventportimpl.h"
#include "../nodekinds.h"
EventPortImpl::EventPortImpl(NODE_KIND kind):BehaviourNode(kind)
{
    setNodeType(NODE_TYPE::IMPLEMENTATION);
    setAcceptsEdgeKind(Edge::EC_DEFINITION);

    updateDefaultData("type", QVariant::String, true);
}

bool EventPortImpl::isInPort()
{
    return getNodeKind() == NODE_KIND::INEVENTPORT_IMPL;
}

bool EventPortImpl::isOutPort()
{
    return getNodeKind() == NODE_KIND::OUTEVENTPORT_IMPL;
}

bool EventPortImpl::canAdoptChild(Node *child)
{
    //Can Only accept 1 child.
    if(hasChildren()){
        return false;
    }

    //Can only adopt AggregateInstances
    if(child->getNodeKind() != NODE_KIND::AGGREGATE_INSTANCE){
        return false;
    }

    return BehaviourNode::canAdoptChild(child);
}

bool EventPortImpl::canAcceptEdge(Edge::EDGE_KIND edgeKind, Node *dst)
{
    return BehaviourNode::canAcceptEdge(edgeKind, dst);
}

