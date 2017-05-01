#include "dataedge.h"
#include "../node.h"

DataEdge::DataEdge(Node *src, Node *dst):Edge(src, dst, EDGE_KIND::DATA)
{
}

DataEdge::DataEdge(EntityFactory* factory):Edge(factory, EDGE_KIND::DATA, "Edge_Data"){
    auto kind = EDGE_KIND::DATA;
	QString kind_string = "Edge_Data";
	RegisterEdgeKind(factory, kind, kind_string, &DataEdge::ConstructEdge);
}

DataEdge *DataEdge::ConstructEdge(Node *src, Node *dst)
{
    DataEdge* edge = 0;
    if(src && dst){
        if(src->canAcceptEdge(EDGE_KIND::DATA, dst)){
            edge = new DataEdge(src, dst);
        }
    }
    return edge;
}
