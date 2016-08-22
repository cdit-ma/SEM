#ifndef CONDITION_H
#define CONDITION_H
#include "behaviournode.h"

class Termination;
class Branch;

class Condition: public BehaviourNode
{
    Q_OBJECT
public:
    Condition();

    Branch* getBranch();
    Termination* getRequiredTermination();
    bool gotTermination();

    bool canAdoptChild(Node*);
    bool canAcceptEdge(Edge::EDGE_CLASS edgeKind, Node *dst);
};

#endif // CONDITION_H
