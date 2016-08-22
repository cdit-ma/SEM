#include "component.h"

Component::Component():Node(Node::NK_COMPONENT)
{
}

bool Component::canAdoptChild(Node *child)
{
    switch(child->getNodeKind()){
    case NK_ATTRIBUTE:
    case NK_INEVENTPORT:
    case NK_OUTEVENTPORT:
        break;
    default:
        return false;
    }
    return Node::canAdoptChild(child);
}

bool Component::canAcceptEdge(Edge::EDGE_CLASS edgeKind, Node *dst)
{
    return false;
}
