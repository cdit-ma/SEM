#ifndef TERMINATION_H
#define TERMINATION_H
#include "behaviournode.h"
class Branch;

class Termination: public BehaviourNode
{
    Q_OBJECT
public:
    Termination();

    Branch* getBranch();

    bool canAdoptChild(Node* child);
    bool canAcceptEdge(Edge::EDGE_KIND edgeKind, Node *dst);
};


#endif // TERMINATION_H
