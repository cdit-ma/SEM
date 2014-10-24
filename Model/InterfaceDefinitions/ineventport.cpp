#include "ineventport.h"
#include "../BehaviourDefinitions/ineventportimpl.h"
#include "../DeploymentDefinitions/ineventportinstance.h"


InEventPort::InEventPort(QString name):Node(name, Node::NT_DEFINITION)
{
}

InEventPort::~InEventPort()
{

}

QString InEventPort::toString()
{
    return QString("InEventPort[%1]: "+this->getName()).arg(this->getID());

}

bool InEventPort::isEdgeLegal(GraphMLContainer *attachableObject)
{
    return true;
}

bool InEventPort::isAdoptLegal(GraphMLContainer *child)
{
    return true;
}

