#ifndef COMPONENT_H
#define COMPONENT_H
#include "../node.h"

class Component : public Node
{
    Q_OBJECT
public:
    Component();
    bool canAdoptChild(Node* child);
    bool canAcceptEdge(Edge::EDGE_CLASS edgeKind, Node *dst);
};

#endif // COMPONENT_H
