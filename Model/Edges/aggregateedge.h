#ifndef AGGREGATEEDGE_H
#define AGGREGATEEDGE_H
#include "../edge.h"

class AggregateEdge : public Edge
{
public:
    AggregateEdge(Node *src, Node *dst);
    ~AggregateEdge();
    QString toString();
};
#endif // AGGREGATEEDGE_H
