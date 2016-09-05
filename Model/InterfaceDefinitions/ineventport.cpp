#include "ineventport.h"

InEventPort::InEventPort():EventPort(NK_INEVENTPORT)
{
}

bool InEventPort::canAdoptChild(Node *child)
{
    return EventPort::canAdoptChild(child);
}

bool InEventPort::canAcceptEdge(Edge::EDGE_KIND edgeKind, Node *dst)
{
    return EventPort::canAcceptEdge(edgeKind, dst);
}
