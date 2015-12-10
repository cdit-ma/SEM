#include "dataedge.h"
#include "../node.h"

DataEdge::DataEdge(Node *src, Node *dst):Edge(src, dst, Edge::EC_DATA)
{
}

DataEdge::~DataEdge()
{

}

QString DataEdge::toString()
{
    return QString("DataEdge[%1]: [" + getSource()->toString() +"] <-> [" + getDestination()->toString() + "]").arg(this->getID());
}

