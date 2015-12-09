#include "eventport.h"
#include "aggregate.h"
#include "aggregateinstance.h"

EventPort::EventPort(bool isInEventPort):Node(Node::NT_DEFINITION)
{
    inEventPort = isInEventPort;
    aggregate = 0;
    addValidEdgeType(Edge::EC_AGGREGATE);
}

EventPort::~EventPort()
{
}

bool EventPort::isInEventPort()
{
    return inEventPort;
}

bool EventPort::isOutEventPort()
{
    return !inEventPort;
}

void EventPort::setAggregate(Aggregate *aggregate)
{
    if(getAggregate() != aggregate){
        this->aggregate = aggregate;
        aggregate->addEventPort(this);
    }
}

Aggregate *EventPort::getAggregate()
{
    return aggregate;
}

void EventPort::unsetAggregate()
{
    if(aggregate){
        aggregate->removeEventPort(this);
        aggregate = 0;
    }
}

bool EventPort::canConnect_AggregateEdge(Node *node)
{
    Aggregate* aggregate = dynamic_cast<Aggregate*>(node);

    if(!aggregate){
        return false;
    }

    if(getAggregate()){
        return false;
    }

    return Node::canConnect_AggregateEdge(aggregate);
}

bool EventPort::canAdoptChild(Node *child)
{
     AggregateInstance* aggregateInstance = dynamic_cast<AggregateInstance*>(child);

     if(!aggregateInstance){
         return false;
     }

     if(hasChildren()){
         return false;
     }

     return Node::canAdoptChild(child);
}
