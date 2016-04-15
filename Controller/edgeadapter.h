#ifndef EDGEADAPTER_H
#define EDGEADAPTER_H
#include "entityadapter.h"
#include "../Model/edge.h"

class EdgeAdapter : public EntityAdapter
{
public:
    EdgeAdapter(Edge* edge);

    int getSourceID();
    int getDestinationID();

    Edge::EDGE_CLASS getEdgeClass();

private:
    int _sourceID;
    int _destinationID;
    Edge::EDGE_CLASS _edgeClass;
    // EntityAdapter interface
};

#endif // EDGEADAPTER_H
