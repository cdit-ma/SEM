#include "variadicparameter.h"

VariadicParameter::VariadicParameter(): Parameter(NK_VARIADIC_PARAMETER)
{
    setDataReciever(true);
    setDataProducer(false);
}

bool VariadicParameter::canAdoptChild(Node *)
{
    return false;
}

bool VariadicParameter::canAcceptEdge(Edge::EDGE_KIND edgeKind, Node *dst)
{
    return Parameter::canAcceptEdge(edgeKind, dst);
}
