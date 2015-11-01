#ifndef INPUTPARAMETER_H
#define INPUTPARAMETER_H
#include "parameter.h"


class InputParameter : public Parameter
{
public:
    InputParameter();

    // Node interface
public:
    bool canAdoptChild(Node *node);
    bool canConnect(Node *node);
};

#endif // INPUTPARAMETER_H
