#include "eventport.h"
#include "aggregate.h"
#include "aggregateinstance.h"

EventPort::EventPort(NODE_KIND kind):Node(kind)
{
    aggregate = 0;
    setNodeType(NT_EVENTPORT);
    setNodeType(NT_DEFINITION);
    setAcceptsEdgeKind(Edge::EC_DEFINITION);
    setAcceptsEdgeKind(Edge::EC_AGGREGATE);
}

bool EventPort::isInPort() const
{
    return getNodeKind() == NK_INEVENTPORT;
}

bool EventPort::isOutPort() const
{
    return getNodeKind() == NK_OUTEVENTPORT;
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

        if(definition && definition->isNodeOfType(NT_EVENTPORT)){
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
    if(child->getNodeKind() != NK_AGGREGATE_INSTANCE){
        return false;
    }

    return Node::canAdoptChild(child);
}

bool EventPort::canAcceptEdge(Edge::EDGE_CLASS edgeKind, Node *dst)
{
    if(!acceptsEdgeKind(edgeKind)){
        return false;
    }
    switch(edgeKind){
    case Edge::EC_AGGREGATE:{
        if(isNodeOfType(NT_INSTANCE)){
            //Don't allow Instances to have aggregate.
            return false;
        }

        if(getAggregate()){
            //Don't allow multiple instances.
            return false;
        }

        if(dst->getNodeKind() != NK_AGGREGATE){
            return false;
        }

        break;
    }
    default:
        break;
    }
    return Node::canAcceptEdge(edgeKind, dst);
}
