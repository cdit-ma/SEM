#include "dataedge.h"
#include "../node.h"

DataEdge::DataEdge(Node *src, Node *dst):Edge(src, dst, Edge::EC_DATA)
{
}

DataEdge *DataEdge::createDataEdge(Node *src, Node *dst)
{
    DataEdge* edge = 0;
    if(src->canAcceptEdge(Edge::EC_DATA, dst)){
        edge = new DataEdge(src, dst);
    }
    return edge;
}
