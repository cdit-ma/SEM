#include "outeventport.h"
#include "../BehaviourDefinitions/outeventportimpl.h"
#include "../DeploymentDefinitions/outeventportinstance.h"
#include "aggregate.h"
#include <QDebug>

OutEventPort::OutEventPort():EventPort()
{
}

OutEventPort::~OutEventPort()
{

}

bool OutEventPort::canConnect(Node* attachableObject)
{
    return EventPort::canConnect(attachableObject);
}

bool OutEventPort::canAdoptChild(Node *child)
{
    return EventPort::canAdoptChild(child);
}

