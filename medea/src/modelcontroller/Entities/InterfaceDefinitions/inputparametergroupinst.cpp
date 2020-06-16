#include "inputparametergroupinst.h"
#include "../../entityfactorybroker.h"
#include "../../entityfactoryregistrybroker.h"
#include "../../entityfactoryregistrybroker.h"

const NODE_KIND node_kind = NODE_KIND::INPUT_PARAMETER_GROUP_INST;
const QString kind_string = "Input Parameter Group Instance";


void MEDEA::InputParameterGroupInst::RegisterWithEntityFactory(::EntityFactoryRegistryBroker& broker){
    broker.RegisterWithEntityFactory(node_kind, kind_string, [](::EntityFactoryBroker& broker, bool is_temp_node){
        return new MEDEA::InputParameterGroupInst(broker, is_temp_node);
    });
}

MEDEA::InputParameterGroupInst::InputParameterGroupInst(::EntityFactoryBroker& broker, bool is_temp) : Node(broker, node_kind, is_temp){
    //Setup State
    setLabelFunctional(false);
    addInstancesDefinitionKind(NODE_KIND::INPUT_PARAMETER_GROUP);
    setChainableDefinition();

    setAcceptsNodeKind(NODE_KIND::ENUM_INST);
    setAcceptsNodeKind(NODE_KIND::AGGREGATE_INST);
    setAcceptsNodeKind(NODE_KIND::MEMBER_INST);
    setAcceptsNodeKind(NODE_KIND::VECTOR_INST);
    setAcceptsNodeKind(NODE_KIND::VOID_TYPE);
    setAcceptsNodeKind(NODE_KIND::VARIADIC_PARAMETER);

    if(is_temp){
        //Break out early for temporary entities
        return;
    }

    //Setup Data
    broker.AttachData(this, KeyName::Type, QVariant::String, ProtectedState::PROTECTED);
    broker.AttachData(this, KeyName::Index, QVariant::Int, ProtectedState::PROTECTED);
    broker.AttachData(this, KeyName::Row, QVariant::Int, ProtectedState::PROTECTED, 0);
    broker.AttachData(this, KeyName::Column, QVariant::Int, ProtectedState::PROTECTED, -1);
}

bool MEDEA::InputParameterGroupInst::canAdoptChild(Node* child){
    auto parent_node = getParentNodeKind();

    switch(child->getNodeKind()){
        case NODE_KIND::VARIADIC_PARAMETER:{
            if(!canAdoptVariadicParameters()){
                return false;
            }
            break;
        }
        default:
            break;
    }
    return Node::canAdoptChild(child);
}

QSet<NODE_KIND> MEDEA::InputParameterGroupInst::getUserConstructableNodeKinds() const{
    auto node_kinds = Node::getUserConstructableNodeKinds();
    if(canAdoptVariadicParameters()){
        node_kinds += NODE_KIND::VARIADIC_PARAMETER;
    }
    return node_kinds;
}

bool MEDEA::InputParameterGroupInst::canAdoptVariadicParameters() const{
    auto parent_node = getParentNode();

    if(parent_node && parent_node->getNodeKind() == NODE_KIND::FUNCTION_CALL){
        if(parent_node->getDataValue(KeyName::IsVariadic).toBool()){
            return true;
        }
    }
    return false;
}