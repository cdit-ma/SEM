#include "eventport.h"
#include "aggregate.h"
#include <QDebug>

EventPort::EventPort():Node(Node::NT_DEFINITION)
{
    aggregate = 0;
}

EventPort::~EventPort()
{

}

QString EventPort::toString()
{
    return QString("EventPort[%1]: "+this->getName()).arg(this->getID());

}

void EventPort::setAggregate(Node *aggregate)
{
    this->aggregate = aggregate;
}

Node *EventPort::getAggregate()
{
    return aggregate;
}

void EventPort::unsetAggregate()
{
    aggregate = 0;
}

bool EventPort::canConnect(Node* attachableObject)
{
    Aggregate* aggregate = dynamic_cast<Aggregate*>(attachableObject);

    if(!aggregate && getAggregate()){
        qWarning() << "Can only connect an EventPort to one aggregate.";
    }

    return Node::canConnect(attachableObject);
}

bool EventPort::canAdoptChild(Node *child)
{
    return false;
}
