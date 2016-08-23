#include "ineventportdelegate.h"

InEventPortDelegate::InEventPortDelegate():EventPortAssembly(NK_INEVENTPORT_DELEGATE)
{
}

bool InEventPortDelegate::canAdoptChild(Node*)
{
    return false;
}

bool InEventPortDelegate::canAcceptEdge(Edge::EDGE_CLASS edgeKind, Node *dst)
{
    return EventPortAssembly::canAcceptEdge(edgeKind, dst);
}

