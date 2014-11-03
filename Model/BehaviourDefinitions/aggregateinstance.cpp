#include "aggregateinstance.h"


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
    return true;
}

bool AggregateInstance::canAdoptChild(Node *child)
{
    return true;
}
