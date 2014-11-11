#include "aggregateinstance.h"
#include "memberinstance.h"
#include "../InterfaceDefinitions/aggregate.h"
#include <QDebug>

AggregateInstance::AggregateInstance():Node(Node::NT_INSTANCE)
{
}

AggregateInstance::~AggregateInstance()
{

}


QString AggregateInstance::toString()
{
    return QString("AggregateInstance[%1]: "+this->getName()).arg(this->getID());
}

bool AggregateInstance::canConnect(Node* attachableObject)
{
    Aggregate* aggregate = dynamic_cast<Aggregate*>(attachableObject);

    if (!aggregate){
        qWarning() << "AggregateInstance can only connect to an Aggregate.";
        return false;
    }
    if(aggregate && getDefinition()){
        qWarning() << "AggregateInstance can only connect to one Aggregate.";
        return false;
    }

    return Node::canConnect(attachableObject);
}

bool AggregateInstance::canAdoptChild(Node *child)
{
    MemberInstance* memberInstance = dynamic_cast<MemberInstance*>(child);

    if(!memberInstance){
        qWarning() << "AggregateInstance can only adopt MemberInstance";
        return false;
    }

    return Node::canAdoptChild(child);
}
