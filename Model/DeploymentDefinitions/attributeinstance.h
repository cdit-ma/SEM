#ifndef ATTRIBUTEINSTANCE_H
#define ATTRIBUTEINSTANCE_H
#include "../node.h"

class AttributeInstance : public Node
{
    Q_OBJECT
public:
    AttributeInstance();

    bool canAdoptChild(Node* child);
    bool canAcceptEdge(Edge::EDGE_CLASS edgeKind, Node *dst);
};

#endif // ATTRIBUTEINSTANCE_H
