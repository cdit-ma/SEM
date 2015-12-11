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
    bool canConnect_DataEdge(Node *node);

};

#endif // VARIABLE_H
