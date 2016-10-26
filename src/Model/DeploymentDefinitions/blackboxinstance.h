#ifndef BLACKBOXINSTANCE_H
#define BLACKBOXINSTANCE_H
#include "../node.h"

class BlackBoxInstance: public Node
{
    Q_OBJECT
public:
    BlackBoxInstance();

    bool canAdoptChild(Node* child);
    bool canAcceptEdge(Edge::EDGE_KIND edgeKind, Node *dst);
};

#endif // BLACKBOXINSTANCE_H
