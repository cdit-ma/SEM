#include "ineventport.h"
#include "../BehaviourDefinitions/ineventportimpl.h"
#include "../DeploymentDefinitions/ineventportinstance.h"
#include "aggregate.h"
#include <QDebug>

InEventPort::InEventPort():EventPort()
{

}

InEventPort::~InEventPort()
{

}

bool InEventPort::canConnect(Node* attachableObject)
{
    return EventPort::canConnect(attachableObject);
}

bool InEventPort::canAdoptChild(Node *child)
{
    return EventPort::canAdoptChild(child);
}
