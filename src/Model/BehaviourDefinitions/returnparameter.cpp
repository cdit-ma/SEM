#include "returnparameter.h"
#include "parameter.h"
#include "variable.h"

ReturnParameter:: ReturnParameter(): Parameter(NK_RETURN_PARAMETER)
{
    setDataProducer(true);
    setDataReciever(false);
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
