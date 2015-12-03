#include "returnparameter.h"
#include "parameter.h"
#include "variable.h"

ReturnParameter::ReturnParameter(): Parameter(false)
{
}

bool ReturnParameter::canAdoptChild(Node *node)
{
    return false;
}

bool ReturnParameter::canConnect(Node *node)
{
    return Parameter::canConnect(node);
}

