#include "memberinstance.h"

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
    return true;
}

bool MemberInstance::canAdoptChild(Node *child)
{
    return true;
}
