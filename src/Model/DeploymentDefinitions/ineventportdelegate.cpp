#include "ineventportdelegate.h"

InEventPortDelegate::InEventPortDelegate():EventPortAssembly(NODE_KIND::INEVENTPORT_DELEGATE)
{
    removeEdgeKind(Edge::EC_DEFINITION);
    removeNodeType(NODE_TYPE::DEFINITION);
}

bool InEventPortDelegate::canAdoptChild(Node*)
{
    return false;
}

bool InEventPortDelegate::canAcceptEdge(Edge::EDGE_KIND edgeKind, Node *dst)
{
    return EventPortAssembly::canAcceptEdge(edgeKind, dst);
}

