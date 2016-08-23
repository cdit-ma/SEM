#ifndef WORKERDEFINITIONS_H
#define WORKERDEFINITIONS_H
#include "node.h"

class WorkerDefinitions : public Node
{
    Q_OBJECT
public:
    WorkerDefinitions();

    VIEW_ASPECT getViewAspect();

    bool canAdoptChild(Node *node);
    virtual bool canAcceptEdge(Edge::EDGE_CLASS edgeKind, Node *dst);
};

#endif // WORKERDEFINITIONS_H
