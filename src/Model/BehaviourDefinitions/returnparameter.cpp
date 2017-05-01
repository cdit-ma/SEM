#include "returnparameter.h"
#include "../nodekinds.h"
#include "parameter.h"
#include "variable.h"

#include "../entityfactory.h"

ReturnParameter::ReturnParameter(EntityFactory* factory) : Parameter(factory, NODE_KIND::RETURN_PARAMETER, "ReturnParameter"){
	auto node_kind = NODE_KIND::INPUT_PARAMETER;
	QString kind_string = "ReturnParameter";
	RegisterNodeKind(factory, node_kind, kind_string, [](){return new ReturnParameter();});
};

ReturnParameter::ReturnParameter(): Parameter(NODE_KIND::RETURN_PARAMETER)
{
    setDataProducer(true);
    setDataReciever(false);
}

bool ReturnParameter::canAdoptChild(Node *)
{
    return false;
}

bool ReturnParameter::canAcceptEdge(EDGE_KIND edgeKind, Node *dst)
{
    return Parameter::canAcceptEdge(edgeKind, dst);
}
