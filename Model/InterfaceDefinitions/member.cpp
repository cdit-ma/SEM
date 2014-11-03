#include "member.h"
#include "../BehaviourDefinitions/memberinstance.h"
#include <QDebug>
Member::Member(): Node(Node::NT_DEFINITION)
{

}

Member::~Member()
{

}

QString Member::toString()
{
    return QString("Member[%1]: "+this->getName()).arg(this->getID());
}

bool Member::canConnect(Node* attachableObject)
{
    MemberInstance* memberInstance = dynamic_cast<MemberInstance*>(attachableObject);

    if (!memberInstance){
        qWarning() << "Member can only connect to a MemberInstance.";
        return false;
    }

    return Node::canConnect(attachableObject);
}

bool Member::canAdoptChild(Node *child)
{
    return false;
}
