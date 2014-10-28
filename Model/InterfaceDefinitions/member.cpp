#include "member.h"

Member::Member(): Node()
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
    return false;
}

bool Member::canAdoptChild(Node *child)
{
    return false;
}
