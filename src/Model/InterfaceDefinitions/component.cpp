#include "component.h"

Component::Component():Node(NODE_KIND::COMPONENT)
{
    setNodeType(NODE_TYPE::DEFINITION);
    setAcceptsEdgeKind(Edge::EC_DEFINITION);

    
}

bool Component::canAdoptChild(Node *child)
{
    switch(child->getNodeKind()){
    case NODE_KIND::ATTRIBUTE:
    case NODE_KIND::INEVENTPORT:
    case NODE_KIND::OUTEVENTPORT:
        break;
    default:
        return false;
    }
    return Node::canAdoptChild(child);
}

bool Component::canAcceptEdge(Edge::EDGE_KIND, Node *)
{
    return false;
}
