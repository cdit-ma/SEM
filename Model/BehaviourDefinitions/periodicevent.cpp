#include "periodicevent.h"
#include "workload.h"
#include "branchstate.h"
#include "outeventportimpl.h"
#include "termination.h"
#include <QDebug>
PeriodicEvent::PeriodicEvent():BehaviourNode(true){}

PeriodicEvent::~PeriodicEvent(){}

bool PeriodicEvent::canConnect(Node* attachableObject)
{
    return BehaviourNode::canConnect(attachableObject);
}

bool PeriodicEvent::canAdoptChild(Node *)
{
    return false;
}
