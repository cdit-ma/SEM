#include "periodicevent.h"
#include <QDebug>
PeriodicEvent::PeriodicEvent(QString name):Node()
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
    Q_UNUSED(attachableObject);
    return false;
}

bool PeriodicEvent::canAdoptChild(Node *child)
{
    Q_UNUSED(child);
    return false;
}
