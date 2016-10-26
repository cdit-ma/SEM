#ifndef BLACKBOX_H
#define BLACKBOX_H
#include "../node.h"

class BlackBox : public Node
{
    Q_OBJECT
public:
    BlackBox();

    bool canAdoptChild(Node* child);
    bool canAcceptEdge(Edge::EDGE_KIND edgeKind, Node *dst);
};

#endif // BLACKBOX_H
