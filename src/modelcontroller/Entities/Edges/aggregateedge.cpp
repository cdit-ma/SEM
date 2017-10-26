#include "aggregateedge.h"
#include "../node.h"
#include "../../edgekinds.h"
#include <QDebug>

AggregateEdge::AggregateEdge(Node *src, Node *dst):Edge(src, dst, EDGE_KIND::AGGREGATE)
{
}

AggregateEdge::AggregateEdge(EntityFactory* factory):Edge(factory, EDGE_KIND::AGGREGATE, "Edge_Aggregate"){
    auto kind = EDGE_KIND::AGGREGATE;
	QString kind_string = "Edge_Aggregate";
	RegisterEdgeKind(factory, kind, kind_string, &AggregateEdge::ConstructEdge);
}

AggregateEdge *AggregateEdge::ConstructEdge(Node *src, Node *dst)
{
    AggregateEdge* edge = 0;
    if(src && dst){
        if(src->canAcceptEdge(EDGE_KIND::AGGREGATE, dst)){
            edge = new AggregateEdge(src, dst);
        }else{
            qCritical() << "Cant accept edge!";
        }
    }else{
        qCritical() << "NO SRC/DST";
    }
    return edge;
}
