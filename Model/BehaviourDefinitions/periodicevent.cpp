#include "periodicevent.h"
#include <QDebug>
PeriodicEvent::PeriodicEvent(QString name):Node(name)
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

bool PeriodicEvent::isEdgeLegal(GraphMLContainer *attachableObject)
{
    Q_UNUSED(attachableObject);
    return false;
}

bool PeriodicEvent::isAdoptLegal(GraphMLContainer *child)
{
    Q_UNUSED(child);
    return false;
}
