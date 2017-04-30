#ifndef CONDITION_H
#define CONDITION_H
#include "behaviournode.h"
#include "../nodekinds.h"

class Termination;
class Branch;

class Condition: public BehaviourNode
{
    Q_OBJECT
public:
    Condition(NODE_KIND kind = NODE_KIND::CONDITION);

    Branch* getBranch();
    Termination* getRequiredTermination();
    bool gotTermination();

    virtual bool canAdoptChild(Node*);
    virtual bool canAcceptEdge(Edge::EDGE_KIND edgeKind, Node *dst);
};

#endif // CONDITION_H
