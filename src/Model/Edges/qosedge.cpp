#include "qosedge.h"
#include "../node.h"
QOSEdge::QOSEdge(Node *src, Node *dst):Edge(src, dst, Edge::EC_QOS)
{
}

QOSEdge *QOSEdge::createQOSEdge(Node *src, Node *dst)
{
    QOSEdge* edge = 0;
    if(src->canAcceptEdge(Edge::EC_QOS, dst)){
        edge = new QOSEdge(src, dst);
    }
    return edge;
}
