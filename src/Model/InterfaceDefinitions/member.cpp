#include "member.h"

Member::Member(): Node(NK_MEMBER)
{
    setNodeType(NT_DEFINITION);
    setAcceptsEdgeKind(Edge::EC_DEFINITION);

    
    updateDefaultData("type", QVariant::String, false, "String");
    updateDefaultData("key", QVariant::Bool, false, false);
}

bool Member::canAdoptChild(Node*)
{
    return false;
}

bool Member::canAcceptEdge(Edge::EDGE_KIND, Node *)
{
    return false;
}
