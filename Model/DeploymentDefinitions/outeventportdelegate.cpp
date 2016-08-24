#include "outeventportdelegate.h"

OutEventPortDelegate::OutEventPortDelegate():EventPortAssembly(NK_OUTEVENTPORT_DELEGATE)
{
    removeEdgeKind(Edge::EC_DEFINITION);
    removeNodeType(NT_DEFINITION);
}

bool OutEventPortDelegate::canAdoptChild(Node*)
{
    return false;
}

bool OutEventPortDelegate::canAcceptEdge(Edge::EDGE_CLASS edgeKind, Node *dst)
{
    return EventPortAssembly::canAcceptEdge(edgeKind, dst);
}

