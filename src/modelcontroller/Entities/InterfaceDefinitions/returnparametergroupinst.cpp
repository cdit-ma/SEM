#include "returnparametergroupinst.h"
#include "../../entityfactorybroker.h"
#include "../../entityfactoryregistrybroker.h"

const NODE_KIND node_kind = NODE_KIND::RETURN_PARAMETER_GROUP_INST;
const QString kind_string = "Return Parameter Group Instance";


void MEDEA::ReturnParameterGroupInst::RegisterWithEntityFactory(::EntityFactoryRegistryBroker& broker){
    broker.RegisterWithEntityFactory(node_kind, kind_string, [](::EntityFactoryBroker& broker, bool is_temp_node){
        return new MEDEA::ReturnParameterGroupInst(broker, is_temp_node);
    });
}

MEDEA::ReturnParameterGroupInst::ReturnParameterGroupInst(::EntityFactoryBroker& broker, bool is_temp) : Node(broker, node_kind, is_temp){
    //Setup State
    setLabelFunctional(false);
    addInstancesDefinitionKind(NODE_KIND::RETURN_PARAMETER_GROUP);
    
    setChainableDefinition();
    setAcceptsNodeKind(NODE_KIND::ENUM_INST);
    setAcceptsNodeKind(NODE_KIND::AGGREGATE_INST);
    setAcceptsNodeKind(NODE_KIND::MEMBER_INST);
    setAcceptsNodeKind(NODE_KIND::VECTOR_INST);
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

bool MEDEA::ReturnParameterGroupInst::canAdoptChild(Node* child)
{
    if(getChildrenCount() > 0 ){
        return false;
    }
    return Node::canAdoptChild(child);
}