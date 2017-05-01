#include "variadicparameter.h"

#include "../entityfactory.h"

VariadicParameter::VariadicParameter(EntityFactory* factory) : Parameter(factory, NODE_KIND::VARIADIC_PARAMETER, "VariadicParameter"){
	auto node_kind = NODE_KIND::VARIADIC_PARAMETER;
	QString kind_string = "VariadicParameter";
	RegisterNodeKind(factory, node_kind, kind_string, [](){return new VariadicParameter();});

    RegisterDefaultData(factory, node_kind, "value", QVariant::String, false);
};

VariadicParameter::VariadicParameter(): Parameter(NODE_KIND::VARIADIC_PARAMETER)
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
