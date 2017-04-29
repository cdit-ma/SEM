#include "inputparameter.h"

InputParameter::InputParameter(): Parameter(NK_INPUT_PARAMETER)
{
    setDataReciever(true);
    setDataProducer(false);

    updateDefaultData("type", QVariant::String, true);
    updateDefaultData("value", QVariant::String, false);
    updateDefaultData("label", QVariant::String, true);
    updateDefaultData("sortOrder", QVariant::Int, true);
}

bool InputParameter::canAdoptChild(Node *)
{
    return false;
}

bool InputParameter::canAcceptEdge(Edge::EDGE_KIND edgeKind, Node *dst)
{
    return Parameter::canAcceptEdge(edgeKind, dst);
}
