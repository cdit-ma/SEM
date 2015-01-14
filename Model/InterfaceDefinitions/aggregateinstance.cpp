#include "aggregateinstance.h"
#include "memberinstance.h"
#include "aggregate.h"
#include <QDebug>

AggregateInstance::AggregateInstance():Node(Node::NT_DEFINSTANCE)
{
}

AggregateInstance::~AggregateInstance()
{
}

bool AggregateInstance::canConnect(Node* attachableObject)
{
    AggregateInstance* aggregateInstance = dynamic_cast<AggregateInstance*>(attachableObject);
    Aggregate* aggregate = dynamic_cast<Aggregate*>(attachableObject);

    if (!aggregate && !aggregateInstance){
        qWarning() << "AggregateInstance can only connect to an Aggregate.";
        return false;
    }
    if(getDefinition() && attachableObject->getDefinition()){
        qWarning() << "AggregateInstance can only connect to one Aggregate.";
        return false;
    }

    return Node::canConnect(attachableObject);
}

bool AggregateInstance::canAdoptChild(Node *child)
{
    MemberInstance* memberInstance = dynamic_cast<MemberInstance*>(child);
    AggregateInstance* aggregateInstance = dynamic_cast<AggregateInstance*>(child);

    if(!memberInstance && !aggregateInstance){
        qWarning() << "AggregateInstance can only adopt MemberInstance or AggregateInstance";
        return false;
    }

    return Node::canAdoptChild(child);
}
