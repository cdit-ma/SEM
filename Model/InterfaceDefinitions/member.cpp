#include "member.h"

Member::Member(): Node(NK_MEMBER)
{
    setNodeType(NT_DEFINITION);
}

bool Member::canAdoptChild(Node*)
{
    return false;
}

bool Member::canAcceptEdge(Edge::EDGE_CLASS edgeKind, Node *dst)
{
    return false;
}
