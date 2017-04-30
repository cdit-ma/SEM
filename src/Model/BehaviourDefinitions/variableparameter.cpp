#include "variableparameter.h"

VariableParameter:: VariableParameter(): Parameter(NODE_KIND::VARIABLE_PARAMETER)
{
    setDataProducer(true);
    setDataReciever(true);
}

bool VariableParameter::canAdoptChild(Node *node)
{
    Q_UNUSED(node);
    return false;
}

bool VariableParameter::canAcceptEdge(Edge::EDGE_KIND edgeKind, Node *dst)
{
    return Parameter::canAcceptEdge(edgeKind, dst);
}
