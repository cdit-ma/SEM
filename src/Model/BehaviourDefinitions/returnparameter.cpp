#include "returnparameter.h"
#include "../nodekinds.h"
#include "parameter.h"
#include "variable.h"

ReturnParameter:: ReturnParameter(): Parameter(NODE_KIND::RETURN_PARAMETER)
{
    setDataProducer(true);
    setDataReciever(false);

    updateDefaultData("type", QVariant::String, true);
    updateDefaultData("label", QVariant::String, true);
    updateDefaultData("sortOrder", QVariant::Int, true);
}

bool ReturnParameter::canAdoptChild(Node *node)
{
    Q_UNUSED(node);
    return false;
}

bool ReturnParameter::canAcceptEdge(Edge::EDGE_KIND edgeKind, Node *dst)
{
    return Parameter::canAcceptEdge(edgeKind, dst);
}
