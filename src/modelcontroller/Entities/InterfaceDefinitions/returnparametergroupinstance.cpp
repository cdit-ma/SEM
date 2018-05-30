#include "returnparametergroupinstance.h"
#include "../../entityfactorybroker.h"
#include "../../entityfactoryregistrybroker.h"

const NODE_KIND node_kind = NODE_KIND::RETURN_PARAMETER_GROUP_INSTANCE;
const QString kind_string = "Return Parameter Group Instance";


void MEDEA::ReturnParameterGroupInstance::RegisterWithEntityFactory(::EntityFactoryRegistryBroker& broker){
    broker.RegisterWithEntityFactory(node_kind, kind_string, [](::EntityFactoryBroker& broker, bool is_temp_node){
        return new MEDEA::ReturnParameterGroupInstance(broker, is_temp_node);
    });
}

MEDEA::ReturnParameterGroupInstance::ReturnParameterGroupInstance(::EntityFactoryBroker& broker, bool is_temp) : Node(broker, node_kind, is_temp){
    //Setup State
    setLabelFunctional(false);
    addInstancesDefinitionKind(NODE_KIND::RETURN_PARAMETER_GROUP);
    
    setChainableDefinition();
    setAcceptsNodeKind(NODE_KIND::ENUM_INSTANCE);
    setAcceptsNodeKind(NODE_KIND::AGGREGATE_INSTANCE);
    setAcceptsNodeKind(NODE_KIND::MEMBER_INSTANCE);
    setAcceptsNodeKind(NODE_KIND::VECTOR_INSTANCE);
    setAcceptsNodeKind(NODE_KIND::VOID_TYPE);

    if(is_temp){
        //Break out early for temporary entities
        return;
    }

    //Setup Data
    broker.AttachData(this, "type", QVariant::String, ProtectedState::PROTECTED);
    broker.AttachData(this, "index", QVariant::Int, ProtectedState::PROTECTED);

    broker.AttachData(this, "row", QVariant::Int, ProtectedState::PROTECTED, 0);
    broker.AttachData(this, "column", QVariant::Int, ProtectedState::PROTECTED, 1);
}

bool MEDEA::ReturnParameterGroupInstance::canAdoptChild(Node* child)
{
    if(getChildrenCount() > 0 ){
        return false;
    }
    return Node::canAdoptChild(child);
}