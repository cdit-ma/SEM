#include "eventport.h"
#include "aggregate.h"
#include "aggregateinstance.h"
#include <QDebug>

EventPort::EventPort():Node(Node::NT_DEFINITION)
{
    aggregate = 0;
}

EventPort::~EventPort()
{

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

Edge::EDGE_CLASS EventPort::canConnect(Node* attachableObject)
{
    Aggregate* aggregate = dynamic_cast<Aggregate*>(attachableObject);

    if(aggregate && getAggregate()){
#ifdef DEBUG_MODE
        qWarning() << "Can only connect an EventPort to one aggregate.";
#endif
        return false;
    }
    if(!aggregate){
#ifdef DEBUG_MODE
        qWarning() << "Can only connect an EventPort to an Aggregate.";
#endif
        return false;
    }


    return Node::canConnect(attachableObject);
}

bool EventPort::canAdoptChild(Node *child)
{
     AggregateInstance* aggregateInstance = dynamic_cast<AggregateInstance*>(child);
     if(aggregateInstance){
         if(this->getChildren(0).count() > 0){
             return false;
         }
     }else{
         return false;
     }

     return Node::canAdoptChild(child);
}
