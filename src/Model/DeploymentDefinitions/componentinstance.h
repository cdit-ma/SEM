#ifndef COMPONENTINSTANCE_H
#define COMPONENTINSTANCE_H
#include "../node.h"

class ComponentInstance : public Node
{
    Q_OBJECT
public:
    ComponentInstance();

    bool canAdoptChild(Node* child);
    bool canAcceptEdge(Edge::EDGE_KIND edgeKind, Node *dst);
};

#endif // COMPONENTINSTANCE_H
