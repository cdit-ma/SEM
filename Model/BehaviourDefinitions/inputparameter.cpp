#include "inputparameter.h"

InputParameter::InputParameter(): Parameter(NK_INPUTPARAMETER)
{
    setDataReciever(true);
    setDataProducer(false);
}

bool InputParameter::canAdoptChild(Node *)
{
    return false;
}

bool InputParameter::canAcceptEdge(Edge::EDGE_KIND edgeKind, Node *dst)
{
    return Parameter::canAcceptEdge(edgeKind, dst);
}
