#include "blackbox.h"

BlackBox::BlackBox():Node(NK_BLACKBOX)
{
    setNodeType(NT_DEFINITION);
    setAcceptsEdgeKind(Edge::EC_DEFINITION);

}

bool BlackBox::canAdoptChild(Node *child)
{
    //Can Only adopt EventPort Definitions
    switch(child->getNodeKind()){
    case NK_INEVENTPORT:
    case NK_OUTEVENTPORT:
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
