#include "eventport.h"
#include "aggregate.h"
#include "aggregateinstance.h"

EventPort::EventPort(NODE_KIND kind):Node(kind)
{
    aggregate = 0;
    setNodeType(NODE_TYPE::EVENTPORT);
    setNodeType(NODE_TYPE::DEFINITION);
    setAcceptsEdgeKind(Edge::EC_DEFINITION);
    setAcceptsEdgeKind(Edge::EC_AGGREGATE);

    
    updateDefaultData("type", QVariant::String, true);
}

bool EventPort::isInPort() const
{
    return getNodeKind() == NODE_KIND::INEVENTPORT;
}

bool EventPort::isOutPort() const
{
    return getNodeKind() == NODE_KIND::OUTEVENTPORT;
}

void EventPort::setAggregate(Aggregate *aggregate)
{
    if(!getAggregate()){
        this->aggregate = aggregate;
    }
}

Aggregate *EventPort::getAggregate()
{
    //Special case.
    if(isInstance()){
        Node* definition = getDefinition(true);

        if(definition && definition->isNodeOfType(NODE_TYPE::EVENTPORT)){
            return ((EventPort*)definition)->getAggregate();
        }
    }
    return aggregate;
}

void EventPort::unsetAggregate()
{
    if(aggregate){
        aggregate = 0;
    }
}

bool EventPort::canAdoptChild(Node *child)
{
    //Can Only accept 1 child.
    if(hasChildren()){
        return false;
    }

    //Can only adopt AggregateInstances
    if(child->getNodeKind() != NODE_KIND::AGGREGATE_INSTANCE){
        return false;
    }

    return Node::canAdoptChild(child);
}

bool EventPort::canAcceptEdge(Edge::EDGE_KIND edgeKind, Node *dst)
{
    if(!acceptsEdgeKind(edgeKind)){
        return false;
    }
    switch(edgeKind){
    case Edge::EC_AGGREGATE:{
        if(isNodeOfType(NODE_TYPE::INSTANCE)){
            //Don't allow Instances to have aggregate.
            return false;
        }

        if(getAggregate()){
            //Don't allow multiple instances.
            return false;
        }

        if(dst->getNodeKind() != NODE_KIND::AGGREGATE){
            return false;
        }

        break;
    }
    default:
        break;
    }
    return Node::canAcceptEdge(edgeKind, dst);
}
