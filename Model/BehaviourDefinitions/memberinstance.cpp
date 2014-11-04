#include "memberinstance.h"
#include "../InterfaceDefinitions/member.h"
#include <QDebug>

MemberInstance::MemberInstance():Node(Node::NT_INSTANCE)
{
}

MemberInstance::~MemberInstance()
{

}


QString MemberInstance::toString()
{
    return QString("MemberInstance[%1]: "+this->getName()).arg(this->getID());
}

bool MemberInstance::canConnect(Node* attachableObject)
{
    Member* member = dynamic_cast<Member*>(attachableObject);

    if (!member){
        qWarning() << "MemberInstance can only connect to a Member.";
        return false;
    }
    if(member && getDefinition()){
        qWarning() << "MemberInstance can only connect to one Member.";
        return false;
    }

    return Node::canConnect(attachableObject);
}

bool MemberInstance::canAdoptChild(Node *child)
{
    return true;
}
