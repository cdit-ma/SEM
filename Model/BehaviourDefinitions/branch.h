#ifndef BRANCH_H
#define BRANCH_H
#include "behaviournode.h"
#include "termination.h"

class Branch: public BehaviourNode
{
    Q_OBJECT
public:
    Branch(NODE_KIND kind);
    Termination* getTermination();

    virtual bool canAdoptChild(Node *node);
    virtual bool canAcceptEdge(Edge::EDGE_CLASS edgeKind, Node *dst);
};

#endif // BRANCH_H
