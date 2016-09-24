#ifndef COMPONENTBEHAVIOUR_H
#define COMPONENTBEHAVIOUR_H
#include "../node.h"

class ComponentImpl: public Node
{
     Q_OBJECT
public:
    ComponentImpl();
    bool canAdoptChild(Node* child);
    bool canAcceptEdge(Edge::EDGE_KIND edgeKind, Node *dst);
};

#endif // COMPONENTBEHAVIOUR_H


