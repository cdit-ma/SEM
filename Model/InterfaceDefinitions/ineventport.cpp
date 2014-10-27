#include "ineventport.h"
#include "../BehaviourDefinitions/ineventportimpl.h"
#include "../DeploymentDefinitions/ineventportinstance.h"


InEventPort::InEventPort(QString name):Node(Node::NT_DEFINITION)
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
    return Node::canConnect(attachableObject);
}

bool InEventPort::canAdoptChild(Node *child)
{
    return true;
}

