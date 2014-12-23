#include "ineventport.h"
#include "../BehaviourDefinitions/ineventportimpl.h"
#include "../DeploymentDefinitions/ineventportinstance.h"
#include "aggregate.h"
#include <QDebug>

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
