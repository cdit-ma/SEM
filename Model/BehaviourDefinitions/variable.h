#ifndef VARIABLE_H
#define VARIABLE_H

#include "behaviournode.h"

class Variable: public BehaviourNode
{
    Q_OBJECT
public:
    Variable();
    ~Variable();
public:
    Edge::EDGE_CLASS canConnect(Node* attachableObject);
    bool canAdoptChild(Node* child);
};

#endif // VARIABLE_H
