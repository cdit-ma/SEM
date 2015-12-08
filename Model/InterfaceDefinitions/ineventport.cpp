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

Edge::EDGE_CLASS InEventPort::canConnect(Node* attachableObject)
{
    return EventPort::canConnect(attachableObject);
}

bool InEventPort::canAdoptChild(Node *child)
{
    return EventPort::canAdoptChild(child);
}
