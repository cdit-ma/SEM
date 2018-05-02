#include "variadicparameter.h"



VariadicParameter::VariadicParameter(EntityFactory* factory) : Parameter(factory, NODE_KIND::VARIADIC_PARAMETER, "VariadicParameter"){
	auto node_kind = NODE_KIND::VARIADIC_PARAMETER;
	QString kind_string = "VariadicParameter";
	RegisterNodeKind(factory, node_kind, kind_string, [](){return new VariadicParameter();});

    RegisterDefaultData(factory, node_kind, "value", QVariant::String, false);
};

VariadicParameter::VariadicParameter(): Parameter(NODE_KIND::VARIADIC_PARAMETER)
{
    setDataReceiver(true);
    setDataProducer(false);
}