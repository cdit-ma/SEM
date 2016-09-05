#ifndef WORKLOAD_H
#define WORKLOAD_H

#include "behaviournode.h"

class Workload: public BehaviourNode
{
    Q_OBJECT
public:
    Workload();

    bool canAdoptChild(Node* child);
    bool canAcceptEdge(Edge::EDGE_KIND edgeKind, Node *dst);
};

#endif // WORKLOAD_H
