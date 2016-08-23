#ifndef ATTRIBUTE_H
#define ATTRIBUTE_H
#include "../node.h"

class Attribute : public Node
{
    Q_OBJECT
public:
    Attribute();
    bool canAdoptChild(Node* child);
    bool canAcceptEdge(Edge::EDGE_CLASS edgeKind, Node *dst);
};

#endif // ATTRIBUTE_H
