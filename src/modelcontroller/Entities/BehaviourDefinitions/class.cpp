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
    
    addInstanceKind(NODE_KIND::CLASS_INSTANCE);

    setAcceptsNodeKind(NODE_KIND::ATTRIBUTE);
    setAcceptsNodeKind(NODE_KIND::TRANSITION_FUNCTION);
    setAcceptsNodeKind(NODE_KIND::FUNCTION);
    setAcceptsNodeKind(NODE_KIND::EXTERNAL_TYPE);
    setAcceptsNodeKind(NODE_KIND::CLASS_INSTANCE);
    setAcceptsNodeKind(NODE_KIND::HEADER);
    setAcceptsNodeKind(NODE_KIND::CALLBACK_FUNCTION);

    if(is_temp){
        //Break out early for temporary entities
        return;
    }

    //Setup Data

    broker.AttachData(this, "icon_prefix", QVariant::String, ProtectedState::UNPROTECTED);
    broker.AttachData(this, "icon", QVariant::String, ProtectedState::UNPROTECTED);
    broker.AttachData(this, "type", QVariant::String, ProtectedState::PROTECTED);
    broker.AttachData(this, "namespace", QVariant::String, ProtectedState::PROTECTED);
    TypeKey::BindNamespaceAndLabelToType(this, true);
};