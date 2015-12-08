#include "inputparameter.h"
#include "parameter.h"

InputParameter::InputParameter(): Parameter(true)
{
}

bool InputParameter::canAdoptChild(Node*)
{
    return false;
}

Edge::EDGE_CLASS InputParameter::canConnect(Node* node)
{
    return Parameter::canConnect(node);
}

