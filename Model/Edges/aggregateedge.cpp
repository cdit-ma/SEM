#include "aggregateedge.h"
#include "../node.h"

AggregateEdge::AggregateEdge(Node *src, Node *dst):Edge(src, dst, Edge::EC_AGGREGATE)
{
}

AggregateEdge::~AggregateEdge()
{

}

QString AggregateEdge::toString()
{
    return QString("AggregateEdge[%1]: [" + getSource()->toString() +"] <-> [" + getDestination()->toString() + "]").arg(this->getID());
}

