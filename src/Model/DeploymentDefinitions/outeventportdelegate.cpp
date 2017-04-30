#include "outeventportdelegate.h"

OutEventPortDelegate::OutEventPortDelegate():EventPortAssembly(NODE_KIND::OUTEVENTPORT_DELEGATE)
{
    removeEdgeKind(Edge::EC_DEFINITION);
    removeNodeType(NODE_TYPE::DEFINITION);
}

bool OutEventPortDelegate::canAdoptChild(Node*)
{
    return false;
}

bool OutEventPortDelegate::canAcceptEdge(Edge::EDGE_KIND edgeKind, Node *dst)
{
    return EventPortAssembly::canAcceptEdge(edgeKind, dst);
}

