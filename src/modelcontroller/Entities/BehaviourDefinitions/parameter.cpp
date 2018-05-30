#include "parameter.h"
#include "../../entityfactorybroker.h"
#include "../../entityfactoryregistrybroker.h"
#include "../../entityfactoryregistrybroker.h"

Parameter::Parameter(EntityFactoryBroker& broker, NODE_KIND node_kind, bool is_temp) : DataNode(broker, node_kind, is_temp){
    //Setup State
    setNodeType(NODE_TYPE::PARAMETER);
    setLabelFunctional(false);

    if(is_temp){
        //Break out early for temporary entities
        return;
    }

    //Setup Data
    broker.AttachData(this, "icon_prefix", QVariant::String, ProtectedState::PROTECTED);
    broker.AttachData(this, "icon", QVariant::String, ProtectedState::PROTECTED);
    broker.AttachData(this, "label", QVariant::String, ProtectedState::PROTECTED);
    broker.AttachData(this, "type", QVariant::String, ProtectedState::PROTECTED);
    broker.AttachData(this, "inner_type", QVariant::String, ProtectedState::PROTECTED);
    broker.AttachData(this, "outer_type", QVariant::String, ProtectedState::PROTECTED);
    broker.AttachData(this, "value", QVariant::String, ProtectedState::PROTECTED);
}

bool Parameter::isInputParameter() const
{
    return getNodeKind() == NODE_KIND::INPUT_PARAMETER;
}

bool Parameter::isVariadicParameter() const
{
    return getNodeKind() == NODE_KIND::VARIADIC_PARAMETER;
}

bool Parameter::isReturnParameter() const
{
    return getNodeKind() == NODE_KIND::RETURN_PARAMETER;
}

bool Parameter::isVariableParameter() const
{
    return getNodeKind() == NODE_KIND::VARIABLE_PARAMETER;
}

bool Parameter::canAcceptEdge(EDGE_KIND edge_kind, Node *dst)
{
    if(canCurrentlyAcceptEdgeKind(edge_kind, dst) == false){
        return false;
    }

    return DataNode::canAcceptEdge(edge_kind, dst);
}
