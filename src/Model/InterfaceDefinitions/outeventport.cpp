#include "outeventport.h"

OutEventPort::OutEventPort():EventPort(NODE_KIND::OUTEVENTPORT)
{

}

bool OutEventPort::canAdoptChild(Node *child)
{
    return EventPort::canAdoptChild(child);
}

bool OutEventPort::canAcceptEdge(Edge::EDGE_KIND edgeKind, Node *dst)
{
    return EventPort::canAcceptEdge(edgeKind, dst);
}

