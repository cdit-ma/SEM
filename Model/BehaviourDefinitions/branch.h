#ifndef BRANCH_H
#define BRANCH_H
#include "behaviournode.h"
#include "termination.h"

class Branch: public BehaviourNode
{
public:
    Branch();

    Termination* getTermination();

    // Node interface
public:
    virtual bool canAdoptChild(Node *node);
    virtual Edge::EDGE_CLASS canConnect(Node *node);
};

#endif // BRANCH_H
