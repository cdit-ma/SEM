#include "edgeadapter.h"
#include "../Model/node.h"
EdgeAdapter::EdgeAdapter(Edge *edge): EntityAdapter(edge)
{
    _edgeClass = edge->getEdgeClass();
    _sourceID = edge->getSource()->getID();
    _destinationID = edge->getDestination()->getID();
}

int EdgeAdapter::getSourceID()
{
    return _sourceID;
}

int EdgeAdapter::getDestinationID()
{
    return _destinationID;
}

Edge::EDGE_CLASS EdgeAdapter::getEdgeClass()
{
    return _edgeClass;
}


