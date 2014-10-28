#include "ineventport.h"
#include "aggregate.h"


InEventPort::InEventPort(QString name):EventPort()
{

}

InEventPort::~InEventPort()
{

}

QString InEventPort::toString()
{
    return QString("InEventPort[%1]: "+this->getName()).arg(this->getID());

}


bool InEventPort::canConnect(Node* attachableObject)
{
    return EventPort::canConnect(attachableObject);
}

bool InEventPort::canAdoptChild(Node *child)
{
    return EventPort::canAdoptChild(child);
}
