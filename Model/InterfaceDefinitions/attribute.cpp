#include "attribute.h"

Attribute::Attribute():Node(Node::NK_ATTRIBUTE)
{
    setNodeType(NT_DEFINITION);
}

bool Attribute::canAdoptChild(Node*)
{
    return false;
}

bool Attribute::canAcceptEdge(Edge::EDGE_CLASS edgeKind, Node *dst)
{
    return false;
}
