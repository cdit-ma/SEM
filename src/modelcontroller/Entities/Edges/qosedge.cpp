#include "qosedge.h"
#include "../node.h"
#include "../../edgekinds.h"

QosEdge::QosEdge(Node *src, Node *dst):Edge(src, dst, EDGE_KIND::QOS)
{
}

QosEdge::QosEdge(EntityFactory* factory):Edge(factory, EDGE_KIND::QOS, "Edge_Qos"){
    auto kind = EDGE_KIND::QOS;
	QString kind_string = "Edge_Qos";
	RegisterEdgeKind(factory, kind, kind_string, &QosEdge::ConstructEdge);
}

QosEdge *QosEdge::ConstructEdge(Node *src, Node *dst)
{
    QosEdge* edge = 0;
    if(src && dst){
        if(src->canAcceptEdge(EDGE_KIND::QOS, dst)){
            edge = new QosEdge(src, dst);
        }
    }
    return edge;
}
