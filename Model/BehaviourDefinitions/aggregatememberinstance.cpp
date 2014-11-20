#include "aggregatememberinstance.h"
#include "memberinstance.h"
#include "../InterfaceDefinitions/aggregatemember.h"
#include <QDebug>
AggregateMemberInstance::AggregateMemberInstance():Node(Node::NT_INSTANCE)
{
}

AggregateMemberInstance::~AggregateMemberInstance()
{

}


QString AggregateMemberInstance::toString()
{
    return QString("AggregateMemberInstance[%1]: "+this->getName()).arg(this->getID());
}

bool AggregateMemberInstance::canConnect(Node* attachableObject)
{
    AggregateMember* aggregateMember = dynamic_cast<AggregateMember*>(attachableObject);

    if (!aggregateMember){
        qWarning() << "AggregateMemberInstance can only connect to an AggregateMember.";
        return false;
    }
    if(aggregateMember && getDefinition()){
        qWarning() << "AggregateMemberInstance can only connect to one AggregateMember.";
        return false;
    }

    return Node::canConnect(attachableObject);
}

bool AggregateMemberInstance::canAdoptChild(Node *child)
{
    MemberInstance* memberInstance = dynamic_cast<MemberInstance*>(child);
    AggregateMemberInstance* aggregateMemberInstance = dynamic_cast<AggregateMemberInstance*>(child);

    if(!memberInstance && !aggregateMemberInstance){
        qWarning() << "AggregateMemberInstance can only adopt MemberInstance or AggregateMemberInstance";
        return false;
    }

    return Node::canAdoptChild(child);
}
