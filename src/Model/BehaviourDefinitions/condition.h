#ifndef CONDITION_H
#define CONDITION_H
#include "behaviournode.h"

class Termination;
class Branch;

class Condition: public BehaviourNode
{
    Q_OBJECT
public:
    Condition(Node::NODE_KIND kind = NK_CONDITION);

    Branch* getBranch();
    Termination* getRequiredTermination();
    bool gotTermination();

    virtual bool canAdoptChild(Node*);
    virtual bool canAcceptEdge(Edge::EDGE_KIND edgeKind, Node *dst);
};

#endif // CONDITION_H
