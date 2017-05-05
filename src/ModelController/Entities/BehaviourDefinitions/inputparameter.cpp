#include "inputparameter.h"

#include "../../edgekinds.h"



InputParameter::InputParameter(EntityFactory* factory) : Parameter(factory, NODE_KIND::INPUT_PARAMETER, "InputParameter"){
	auto node_kind = NODE_KIND::INPUT_PARAMETER;
	QString kind_string = "InputParameter";
	RegisterNodeKind(factory, node_kind, kind_string, [](){return new InputParameter();});

    RegisterDefaultData(factory, node_kind, "value", QVariant::String, false);
};

InputParameter::InputParameter(): Parameter(NODE_KIND::INPUT_PARAMETER)
{
    setDataReciever(true);
    setDataProducer(false);
}

bool InputParameter::canAdoptChild(Node *)
{
    return false;
}

bool InputParameter::canAcceptEdge(EDGE_KIND edgeKind, Node *dst)
{
    return Parameter::canAcceptEdge(edgeKind, dst);
}
