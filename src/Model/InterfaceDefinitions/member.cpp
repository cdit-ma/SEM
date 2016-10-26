#include "member.h"

Member::Member(): Node(NK_MEMBER)
{
    setNodeType(NT_DEFINITION);
    setAcceptsEdgeKind(Edge::EC_DEFINITION);
}

bool Member::canAdoptChild(Node*)
{
    return false;
}

bool Member::canAcceptEdge(Edge::EDGE_KIND, Node *)
{
    return false;
}
