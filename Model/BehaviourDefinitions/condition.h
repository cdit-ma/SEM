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
    ~Condition();

    Branch* getBranch();
    Termination* getTermination();

public:
    bool canAdoptChild(Node*);
    bool canConnect_WorkflowEdge(Node *node);
};

#endif // CONDITION_H
