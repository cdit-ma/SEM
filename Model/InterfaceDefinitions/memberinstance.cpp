#include "memberinstance.h"
#include "../InterfaceDefinitions/member.h"
#include <QDebug>

MemberInstance::MemberInstance():Node(Node::NT_DEFINSTANCE)
{
}

MemberInstance::~MemberInstance()
{

}

bool MemberInstance::canConnect(Node* attachableObject)
{
    MemberInstance* memberInstance = dynamic_cast<MemberInstance*>(attachableObject);
    Member* member = dynamic_cast<Member*>(attachableObject);

    if (!member && !memberInstance){
        qWarning() << "MemberInstance can only connect to an MemberInstance or Member.";
        return false;
    }
    if(getDefinition() && attachableObject->getDefinition()){
        qWarning() << "MemberInstance can only connect to one MemberInstance.";
        return false;
    }

    return Node::canConnect(attachableObject);
}

bool MemberInstance::canAdoptChild(Node *child)
{
    Q_UNUSED(child);
    return false;
}
