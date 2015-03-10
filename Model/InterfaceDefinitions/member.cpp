#include "member.h"
#include <QDebug>
Member::Member(): Node(Node::NT_DEFINITION)
{

}

Member::~Member()
{

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
