#ifndef AGGREGATEEDGE_H
#define AGGREGATEEDGE_H
#include "../edge.h"

class AggregateEdge : public Edge
{
    Q_OBJECT

protected:
    AggregateEdge(Node *src, Node *dst);

public:
    static AggregateEdge* createAggregateEdge(Node* src, Node* dst);
};
#endif // AGGREGATEEDGE_H
