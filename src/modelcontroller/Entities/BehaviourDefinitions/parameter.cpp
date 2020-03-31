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
    broker.AttachData(this, KeyName::IconPrefix, QVariant::String, ProtectedState::PROTECTED);
    broker.AttachData(this, KeyName::Icon, QVariant::String, ProtectedState::PROTECTED);
    broker.AttachData(this, KeyName::Label, QVariant::String, ProtectedState::PROTECTED);
    broker.AttachData(this, KeyName::Type, QVariant::String, ProtectedState::PROTECTED);
    broker.AttachData(this, KeyName::InnerType, QVariant::String, ProtectedState::PROTECTED);
    broker.AttachData(this, KeyName::OuterType, QVariant::String, ProtectedState::PROTECTED);
    broker.AttachData(this, KeyName::Value, QVariant::String, ProtectedState::PROTECTED);
}

bool Parameter::canAcceptEdge(EDGE_KIND edge_kind, Node *dst)
{
    if(canCurrentlyAcceptEdgeKind(edge_kind, dst) == false){
        return false;
    }

    return DataNode::canAcceptEdge(edge_kind, dst);
}
