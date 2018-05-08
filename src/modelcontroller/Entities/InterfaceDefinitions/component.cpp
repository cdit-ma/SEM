#include "component.h"
#include "../data.h"
#include "../Keys/typekey.h"
#include "../../entityfactory.h"

const NODE_KIND node_kind = NODE_KIND::COMPONENT;
const QString kind_string = "Component";

void Component::RegisterWithEntityFactory(EntityFactory& factory){
    Node::RegisterWithEntityFactory(factory, node_kind, kind_string, [](EntityFactory& factory, bool is_temp_node){
        return new Component(factory, is_temp_node);
    });
}

Component::Component(EntityFactory& factory, bool is_temp) : Node(factory, node_kind, is_temp){
    if(is_temp){
        return;
    }

    //Setup State
    addInstanceKind(NODE_KIND::COMPONENT_INSTANCE);
    addImplKind(NODE_KIND::COMPONENT_IMPL);
    setAcceptsNodeKind(NODE_KIND::ATTRIBUTE);
    setAcceptsNodeKind(NODE_KIND::INEVENTPORT);
    setAcceptsNodeKind(NODE_KIND::OUTEVENTPORT);
    setAcceptsNodeKind(NODE_KIND::SERVER_PORT);
    setAcceptsNodeKind(NODE_KIND::CLIENT_PORT);

    //Setup Data
    factory.AttachData(this, "comment", QVariant::String, "");
    factory.AttachData(this, "namespace", QVariant::String, "", true);
    factory.AttachData(this, "type", QVariant::String, "", true);
}

void Component::DataAdded(Data* data){
    Node::DataAdded(data);

    auto key_name = data->getKeyName();

    if(key_name == "label" || key_name == "namespace" || key_name == "type"){
        TypeKey::BindNamespaceAndLabelToType(this, true);
    }
}
