#ifndef TERMINATION_H
#define TERMINATION_H
#include "behaviournode.h"
class Branch;

class Termination: public BehaviourNode
{
    Q_OBJECT
public:
    Termination();
    ~Termination();

    Branch* getBranch();
    WorkflowEdge* getLeftWorkflowEdge();
public:
    bool canAdoptChild(Node*);
    bool canConnect_WorkflowEdge(Node *node);
};


#endif // TERMINATION_H
