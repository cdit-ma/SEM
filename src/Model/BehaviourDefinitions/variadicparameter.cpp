#include "variadicparameter.h"

VariadicParameter::VariadicParameter(): Parameter(NODE_KIND::VARIADIC_PARAMETER)
{
    setDataReciever(true);
    setDataProducer(false);

    
    updateDefaultData("value", QVariant::String);
}

bool VariadicParameter::canAdoptChild(Node *)
{
    return false;
}

bool VariadicParameter::canAcceptEdge(Edge::EDGE_KIND edgeKind, Node *dst)
{
    return Parameter::canAcceptEdge(edgeKind, dst);
}
