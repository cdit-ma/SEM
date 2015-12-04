#ifndef PARAMETER_H
#define PARAMETER_H
#include "behaviournode.h"
class BehaviourNode;

class Parameter: public BehaviourNode
{
public:
    Parameter(bool isInput=false);

    bool isInputParameter();
    bool isReturnParameter();
    bool hasConnection();

    // Node interface
public:
    bool canAdoptChild(Node *node);
    bool canConnect(Node *node);
private:
    bool inputParameter;
};

#endif // PARAMETER_H
