#include "eventportimpl.h"

#include "../../edgekinds.h"

EventPortImpl::EventPortImpl(EntityFactory* factory, NODE_KIND kind, QString kind_str) : BehaviourNode(factory, kind, kind_str){
    //Register DefaultData
    RegisterDefaultData(factory, kind, "type", QVariant::String, true);
};

EventPortImpl::EventPortImpl(NODE_KIND kind):BehaviourNode(kind)
{
    setNodeType(NODE_TYPE::IMPLEMENTATION);
    setAcceptsEdgeKind(EDGE_KIND::DEFINITION);
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

bool EventPortImpl::canAcceptEdge(EDGE_KIND edgeKind, Node *dst)
{
    return BehaviourNode::canAcceptEdge(edgeKind, dst);
}

