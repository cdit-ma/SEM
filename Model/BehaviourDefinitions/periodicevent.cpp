#include "periodicevent.h"
#include <QDebug>
PeriodicEvent::PeriodicEvent():Node()
{
    //qCritical() << "Constructed PeriodicEvent: "<< this->getName();
}

PeriodicEvent::~PeriodicEvent()
{
    //Destructor
}

QString PeriodicEvent::toString()
{
    return QString("PeriodicEvent[%1]: "+this->getName()).arg(this->getID());
}

bool PeriodicEvent::canConnect(Node* attachableObject)
{
    return Node::canConnect(attachableObject);
}

bool PeriodicEvent::canAdoptChild(Node *child)
{
    return Node::canAdoptChild(child);
}
