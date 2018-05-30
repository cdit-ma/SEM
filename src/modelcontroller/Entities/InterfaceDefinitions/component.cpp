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
    addInstanceKind(NODE_KIND::COMPONENT_INSTANCE);
    addImplKind(NODE_KIND::COMPONENT_IMPL);
    setAcceptsNodeKind(NODE_KIND::ATTRIBUTE);
    setAcceptsNodeKind(NODE_KIND::INEVENTPORT);
    setAcceptsNodeKind(NODE_KIND::OUTEVENTPORT);
    setAcceptsNodeKind(NODE_KIND::SERVER_PORT);
    setAcceptsNodeKind(NODE_KIND::CLIENT_PORT);

    if(is_temp){
        //Break out early for temporary entities
        return;
    }
    
    //Setup Data
    broker.AttachData(this, "comment", QVariant::String, ProtectedState::UNPROTECTED);
    broker.AttachData(this, "namespace", QVariant::String, ProtectedState::PROTECTED);
    broker.AttachData(this, "type", QVariant::String, ProtectedState::PROTECTED);
    TypeKey::BindNamespaceAndLabelToType(this, true);
}
