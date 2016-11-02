#include "aggregateedge.h"
#include "../node.h"

AggregateEdge::AggregateEdge(Node *src, Node *dst):Edge(src, dst, Edge::EC_AGGREGATE)
{
}

AggregateEdge *AggregateEdge::createAggregateEdge(Node *src, Node *dst)
{
    AggregateEdge* edge = 0;
    if(src->canAcceptEdge(Edge::EC_AGGREGATE, dst)){
        edge = new AggregateEdge(src, dst);
    }
    return edge;
}
