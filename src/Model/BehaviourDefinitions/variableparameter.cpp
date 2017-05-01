#include "variableparameter.h"


VariableParameter::VariableParameter(EntityFactory* factory) : Parameter(factory, NODE_KIND::VARIABLE_PARAMETER, "VariableParameter"){
	auto node_kind = NODE_KIND::VARIABLE_PARAMETER;
	QString kind_string = "VariableParameter";
	RegisterNodeKind(factory, node_kind, kind_string, [](){return new VariableParameter();});

    RegisterDefaultData(factory, kind, "value", QVariant::String, false);
};

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
