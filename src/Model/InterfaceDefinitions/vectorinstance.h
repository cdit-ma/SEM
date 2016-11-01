#ifndef VECTORINSTANCE_H
#define VECTORINSTANCE_H
#include "datanode.h"

class VectorInstance : public DataNode
{
    Q_OBJECT
public:
    VectorInstance();

    bool canAdoptChild(Node* child);
    bool canAcceptEdge(Edge::EDGE_KIND edgeKind, Node *dst);
};


#endif // AGGREGATE_H

