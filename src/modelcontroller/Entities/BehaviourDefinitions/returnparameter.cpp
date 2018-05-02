#include "returnparameter.h"

#include "../../edgekinds.h"
#include "parameter.h"
#include "variable.h"



ReturnParameter::ReturnParameter(EntityFactory* factory) : Parameter(factory, NODE_KIND::RETURN_PARAMETER, "ReturnParameter"){
	auto node_kind = NODE_KIND::RETURN_PARAMETER;
	QString kind_string = "ReturnParameter";
	RegisterNodeKind(factory, node_kind, kind_string, [](){return new ReturnParameter();});
};

ReturnParameter::ReturnParameter(): Parameter(NODE_KIND::RETURN_PARAMETER)
{
    setDataProducer(true);
    setDataReceiver(false);
}