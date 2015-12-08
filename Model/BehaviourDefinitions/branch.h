#ifndef BRANCH_H
#define BRANCH_H
#include "behaviournode.h"
#include "termination.h"

class Branch: public BehaviourNode
{
    Q_OBJECT
public:
    Branch();
    Termination* getTermination();

    virtual bool canAdoptChild(Node *node);
    bool canConnect_WorkflowEdge(Node *node);
};

#endif // BRANCH_H
