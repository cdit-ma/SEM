#include "eventport.h"
#include <QDebug>

EventPort::EventPort(QString name):Node(this->nodeKind,name)
{
    qDebug() << "Constructed EventPort: "<< this->getName();
}

EventPort::~EventPort()
{
    //Destructor
}

QString EventPort::toString()
{
    return QString("EventPort[%1]: "+this->getName()).arg(this->getID());
}
