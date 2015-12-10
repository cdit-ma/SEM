#ifndef VARIABLE_H
#define VARIABLE_H
#include "behaviournode.h"

class Variable: public BehaviourNode
{
    Q_OBJECT
public:
    Variable();
    ~Variable();

    bool canAdoptChild(Node* child);

};

#endif // VARIABLE_H
