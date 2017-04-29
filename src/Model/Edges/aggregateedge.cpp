#include "aggregateedge.h"
#include "../node.h"

AggregateEdge::AggregateEdge(Node *src, Node *dst):Edge(src, dst, Edge::EC_AGGREGATE)
{
}

AggregateEdge *AggregateEdge::createAggregateEdge(Node *src, Node *dst)
{
    AggregateEdge* edge = 0;
    if(src && dst){
        if(src->canAcceptEdge(Edge::EC_AGGREGATE, dst)){
            edge = new AggregateEdge(src, dst);
        }
    }else if(!src && !dst){
        //Allow an empty edge
        edge = new AggregateEdge(0, 0);
    }
    return edge;
}
