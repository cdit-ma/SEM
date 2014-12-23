#include "member.h"
#include "memberinstance.h"
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
    Q_UNUSED(attachableObject);
    return false;
}

bool Member::canAdoptChild(Node *child)
{
    Q_UNUSED(child);
    return false;
}
