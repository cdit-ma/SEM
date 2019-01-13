#include "returnparametergroup.h"
#include "../../entityfactorybroker.h"
#include "../../entityfactoryregistrybroker.h"
#include "../../entityfactoryregistrybroker.h"

const NODE_KIND node_kind = NODE_KIND::RETURN_PARAMETER_GROUP;
const QString kind_string = "Return Parameter Group";

void MEDEA::ReturnParameterGroup::RegisterWithEntityFactory(::EntityFactoryRegistryBroker& broker){
    broker.RegisterWithEntityFactory(node_kind, kind_string, [](::EntityFactoryBroker& broker, bool is_temp_node){
        return new MEDEA::ReturnParameterGroup(broker, is_temp_node);
    });
}

MEDEA::ReturnParameterGroup::ReturnParameterGroup(::EntityFactoryBroker& broker, bool is_temp) : Node(broker, node_kind, is_temp){
    //Setup State
    setLabelFunctional(false);
    addInstanceKind(NODE_KIND::RETURN_PARAMETER_GROUP_INST);

    if(is_temp){
        //Break out early for temporary entities
        return;
    }

    //Setup Data
    broker.AttachData(this, "label", QVariant::String, ProtectedState::PROTECTED, "Return Parameters");
    broker.AttachData(this, "type", QVariant::String, ProtectedState::PROTECTED);
    broker.AttachData(this, "index", QVariant::Int, ProtectedState::PROTECTED);

    broker.AttachData(this, "row", QVariant::Int, ProtectedState::PROTECTED, 0);
    broker.AttachData(this, "column", QVariant::Int, ProtectedState::PROTECTED, 1);
}

bool MEDEA::ReturnParameterGroup::canAdoptChild(Node* child)
{ 
    //Server interface Input Parameter Groups can only have a singular child
    if(getChildrenCount() > 0 ){
        return false;
    }
    return Node::canAdoptChild(child);
}

void MEDEA::ReturnParameterGroup::parentSet(Node* parent){
    auto parent_kind = parent->getNodeKind();
    if(parent_kind == NODE_KIND::SERVER_INTERFACE){
        //Only allow AggregateInstances
        setAcceptsNodeKind(NODE_KIND::AGGREGATE_INST);
        setAcceptsNodeKind(NODE_KIND::VOID_TYPE);
    }else{
        setAcceptsNodeKind(NODE_KIND::AGGREGATE_INST);
        setAcceptsNodeKind(NODE_KIND::ENUM_INST);
        setAcceptsNodeKind(NODE_KIND::MEMBER);
        setAcceptsNodeKind(NODE_KIND::VECTOR);
        setAcceptsNodeKind(NODE_KIND::VOID_TYPE);
    }
}