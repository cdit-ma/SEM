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
public:
    bool canAdoptChild(Node*);
};


#endif // TERMINATION_H
