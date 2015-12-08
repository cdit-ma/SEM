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
    Edge::EDGE_CLASS canConnect(Node* attachableObject);
    bool canAdoptChild(Node*);
};

#endif // CONDITION_H
