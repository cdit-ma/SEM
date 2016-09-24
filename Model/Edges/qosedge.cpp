#include "qosedge.h"
#include "../node.h"
QOSEdge::QOSEdge(Node *src, Node *dst):Edge(src, dst, Edge::EC_QOS)
{
}

QOSEdge::~QOSEdge()
{
}

QString QOSEdge::toString()
{
    return QString("QOSEdge[%1]: [" + getSource()->toString() +"] <-> [" + getDestination()->toString() + "]").arg(this->getID());
}
