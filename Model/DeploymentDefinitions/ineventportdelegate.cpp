#include "ineventportdelegate.h"

InEventPortDelegate::InEventPortDelegate():EventPortAssembly(NK_INEVENTPORT_DELEGATE)
{
    removeEdgeKind(Edge::EC_DEFINITION);
    removeNodeType(NT_DEFINITION);
}

bool InEventPortDelegate::canAdoptChild(Node*)
{
    return false;
}

bool InEventPortDelegate::canAcceptEdge(Edge::EDGE_KIND edgeKind, Node *dst)
{
    return EventPortAssembly::canAcceptEdge(edgeKind, dst);
}

