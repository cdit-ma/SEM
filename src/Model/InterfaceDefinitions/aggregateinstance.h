#ifndef AGGREGATEINSTANCE_H
#define AGGREGATEINSTANCE_H
#include "datanode.h"

class AggregateInstance : public DataNode
{
    Q_OBJECT
public:
    AggregateInstance();
    bool canAdoptChild(Node* child);
    bool canAcceptEdge(Edge::EDGE_KIND edgeKind, Node *dst);
};

#endif // AGGREGATEINSTANCE_H
