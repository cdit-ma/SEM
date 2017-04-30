#include "blackbox.h"

BlackBox::BlackBox():Node(NODE_KIND::BLACKBOX)
{
    setNodeType(NODE_TYPE::DEFINITION);
    setAcceptsEdgeKind(Edge::EC_DEFINITION);

    
}

bool BlackBox::canAdoptChild(Node *child)
{
    //Can Only adopt EventPort Definitions
    switch(child->getNodeKind()){
    case NODE_KIND::INEVENTPORT:
    case NODE_KIND::OUTEVENTPORT:
        break;
    default:
        return false;
    }

    return Node::canAdoptChild(child);
}

bool BlackBox::canAcceptEdge(Edge::EDGE_KIND, Node *)
{
    return false;
}
