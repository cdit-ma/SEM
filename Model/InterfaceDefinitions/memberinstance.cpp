#include "memberinstance.h"
#include "../InterfaceDefinitions/member.h"
#include <QDebug>

MemberInstance::MemberInstance():Node(Node::NT_DEFINSTANCE)
{
}

MemberInstance::~MemberInstance()
{

}

Edge::EDGE_CLASS MemberInstance::canConnect(Node* attachableObject)
{
    MemberInstance* memberInstance = dynamic_cast<MemberInstance*>(attachableObject);
    Member* member = dynamic_cast<Member*>(attachableObject);

    if (!member && !memberInstance){
#ifdef DEBUG_MODE
        qWarning() << "MemberInstance can only connect to an MemberInstance or Member.";
#endif
        return false;
    }

    if(getDefinition()){
#ifdef DEBUG_MODE
        qWarning() << "MemberInstance can only connect to one MemberInstance/Member.";
#endif
        return false;
    }



    return Node::canConnect(attachableObject);
}

bool MemberInstance::canAdoptChild(Node *child)
{
    Q_UNUSED(child);
    return false;
}
