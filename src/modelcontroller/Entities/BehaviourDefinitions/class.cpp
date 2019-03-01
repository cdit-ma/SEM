#include "class.h"
#include "../../entityfactorybroker.h"
#include "../../entityfactoryregistrybroker.h"
#include "../Keys/typekey.h"

const NODE_KIND node_kind = NODE_KIND::CLASS;
const QString kind_string = "Class";

void MEDEA::Class::RegisterWithEntityFactory(::EntityFactoryRegistryBroker& broker){
    broker.RegisterWithEntityFactory(node_kind, kind_string, [](::EntityFactoryBroker& broker, bool is_temp_node){
        return new MEDEA::Class(broker, is_temp_node);
        });
}

MEDEA::Class::Class(::EntityFactoryBroker& broker, bool is_temp) : Node(broker, node_kind, is_temp){
    //Setup State
    setNodeType(NODE_TYPE::TOP_BEHAVIOUR_CONTAINER);
    setNodeType(NODE_TYPE::BEHAVIOUR_CONTAINER);
    
    addInstanceKind(NODE_KIND::CLASS_INST);

    setAcceptsNodeKind(NODE_KIND::ATTRIBUTE);
    setAcceptsNodeKind(NODE_KIND::TRANSITION_FUNCTION);
    setAcceptsNodeKind(NODE_KIND::FUNCTION);
    setAcceptsNodeKind(NODE_KIND::EXTERNAL_TYPE);
    setAcceptsNodeKind(NODE_KIND::CLASS_INST);
    setAcceptsNodeKind(NODE_KIND::HEADER);
    setAcceptsNodeKind(NODE_KIND::CALLBACK_FUNCTION);

    if(is_temp){
        //Break out early for temporary entities
        return;
    }

    //Setup Data

    broker.AttachData(this, KeyName::IconPrefix, QVariant::String, ProtectedState::UNPROTECTED);
    broker.AttachData(this, KeyName::Icon, QVariant::String, ProtectedState::UNPROTECTED);
    broker.AttachData(this, KeyName::Type, QVariant::String, ProtectedState::PROTECTED);
    broker.AttachData(this, KeyName::Namespace, QVariant::String, ProtectedState::PROTECTED);
    broker.AttachData(this, KeyName::IsWorker, QVariant::Bool, ProtectedState::PROTECTED);
    TypeKey::BindNamespaceAndLabelToType(this, true);
};

void MEDEA::Class::parentSet(Node* parent){
    bool is_worker = false;
    switch(getViewAspect()){
        case VIEW_ASPECT::WORKERS:{
            is_worker = true;
            break;
        }
        default:{
            is_worker = false;
            break;
        }
    }
    getFactoryBroker().AttachData(this, KeyName::IsWorker, QVariant::Bool, ProtectedState::PROTECTED, is_worker);
}


bool MEDEA::Class::canAdoptChild(Node* child)
{
    auto child_kind = child->getNodeKind();
    switch(child_kind){
        case NODE_KIND::CALLBACK_FUNCTION:{
            //Only allow Workers to have Callback Function
            if(getDataValue(KeyName::IsWorker).toBool() == false){
                return false;
            }
            break;
        }
        default:
            break;
    }
    return Node::canAdoptChild(child);
}