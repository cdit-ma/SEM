#include "component.h"
#include "../data.h"
#include "../Keys/typekey.h"
#include "../../entityfactorybroker.h"
#include "../../entityfactoryregistrybroker.h"
#include "../../entityfactoryregistrybroker.h"

const NODE_KIND node_kind = NODE_KIND::COMPONENT;
const QString kind_string = "Component";

void Component::RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker){
    broker.RegisterWithEntityFactory(node_kind, kind_string, [](EntityFactoryBroker& broker, bool is_temp_node){
        return new Component(broker, is_temp_node);
    });
}

Component::Component(EntityFactoryBroker& broker, bool is_temp) : Node(broker, node_kind, is_temp){
    //Setup State
    addInstanceKind(NODE_KIND::COMPONENT_INST);
    addImplKind(NODE_KIND::COMPONENT_IMPL);
    setAcceptsNodeKind(NODE_KIND::ATTRIBUTE);
    
    setAcceptsNodeKind(NODE_KIND::PORT_SUBSCRIBER);
    setAcceptsNodeKind(NODE_KIND::PORT_PUBLISHER);
    setAcceptsNodeKind(NODE_KIND::PORT_REPLIER);
    setAcceptsNodeKind(NODE_KIND::PORT_REQUESTER);

    if(is_temp){
        //Break out early for temporary entities
        return;
    }
    
    //Setup Data
    broker.AttachData(this, KeyName::Comment, QVariant::String, ProtectedState::UNPROTECTED);
    broker.AttachData(this, KeyName::Namespace, QVariant::String, ProtectedState::PROTECTED);
    broker.AttachData(this, KeyName::Type, QVariant::String, ProtectedState::PROTECTED);
    TypeKey::BindNamespaceAndLabelToType(this, true);
}
