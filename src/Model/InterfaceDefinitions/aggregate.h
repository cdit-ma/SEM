#ifndef AGGREGATE_H
#define AGGREGATE_H
#include "../node.h"

class Aggregate : public Node
{
    Q_OBJECT
public:
    Aggregate();
    bool canAdoptChild(Node* child);
    bool canAcceptEdge(Edge::EDGE_KIND edgeKind, Node *dst);
};


#endif // AGGREGATE_H
