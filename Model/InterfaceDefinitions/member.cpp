#include "member.h"
#include <QDebug>
Member::Member(): Node(Node::NT_DEFINITION)
{

}

Member::~Member()
{

}

Edge::EDGE_CLASS Member::canConnect(Node* attachableObject)
{
    Q_UNUSED(attachableObject);
    return false;
}

bool Member::canAdoptChild(Node *child)
{
    Q_UNUSED(child);
    return false;
}
