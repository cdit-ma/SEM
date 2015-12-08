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

Edge::EDGE_CLASS ReturnParametercanConnect(Node *node)
{
    return Parameter::canConnect(node);
}

