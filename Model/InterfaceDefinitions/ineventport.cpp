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
    InEventPortImpl* inEventPortImpl = dynamic_cast<InEventPortImpl*>(attachableObject);
    InEventPortInstance* inEventPortInstance = dynamic_cast<InEventPortInstance*>(attachableObject);
    Aggregate* aggregate = dynamic_cast<Aggregate*>(attachableObject);


    if(inEventPortImpl && getImplementations().size() != 0){
        qWarning() << "A InEventPort can only connect to one inEventPortImpl. Detach First!";
        return false;
    }

    if(!aggregate && !inEventPortImpl && !inEventPortInstance){
        qWarning() << "A InEventPort can only connect to a Aggregate, InEventPortImpl or InEventPortInstance.";
        return false;
    }

    return EventPort::canConnect(attachableObject);
}

bool InEventPort::canAdoptChild(Node *child)
{
    return EventPort::canAdoptChild(child);
}
