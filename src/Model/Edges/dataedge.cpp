#include "dataedge.h"
#include "../node.h"

DataEdge::DataEdge(Node *src, Node *dst):Edge(src, dst, Edge::EC_DATA)
{
}

DataEdge *DataEdge::createDataEdge(Node *src, Node *dst)
{
    DataEdge* edge = 0;
    
    if(src && dst){
        if(src->canAcceptEdge(Edge::EC_DATA, dst)){
            edge = new DataEdge(src, dst);
        }
    }else if(!src && !dst){
        //Allow an empty edge
        edge = new DataEdge(0, 0);
    }

    return edge;
}
