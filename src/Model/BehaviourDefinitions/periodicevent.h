#ifndef PERIODICEVENT_H
#define PERIODICEVENT_H
#include "behaviournode.h"

class PeriodicEvent: public BehaviourNode
{
    Q_OBJECT
public:
    PeriodicEvent();
    bool canAdoptChild(Node* child);
    bool canAcceptEdge(Edge::EDGE_KIND edgeKind, Node *dst);
};

#endif // PERIODICEVENT_H

