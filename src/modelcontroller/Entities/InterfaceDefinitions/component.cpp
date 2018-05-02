#include "component.h"
#include "../data.h"
#include "../Keys/typekey.h"

Component::Component(EntityFactory* factory) : Node(factory, NODE_KIND::COMPONENT, "Component"){
	auto node_kind = NODE_KIND::COMPONENT;
	QString kind_string = "Component";
	RegisterNodeKind(factory, node_kind, kind_string, [](){return new Component();});

    RegisterDefaultData(factory, node_kind, "comment", QVariant::String);
    RegisterDefaultData(factory, node_kind, "namespace", QVariant::String, true);
    RegisterDefaultData(factory, node_kind, "type", QVariant::String, true);
};

Component::Component():Node(NODE_KIND::COMPONENT)
{
    addInstanceKind(NODE_KIND::COMPONENT_INSTANCE);
    addImplKind(NODE_KIND::COMPONENT_IMPL);

    setAcceptsNodeKind(NODE_KIND::ATTRIBUTE);
    setAcceptsNodeKind(NODE_KIND::INEVENTPORT);
    setAcceptsNodeKind(NODE_KIND::OUTEVENTPORT);
    setAcceptsNodeKind(NODE_KIND::SERVER_PORT);
    setAcceptsNodeKind(NODE_KIND::CLIENT_PORT);
}

void Component::DataAdded(Data* data){
    Node::DataAdded(data);

    auto key_name = data->getKeyName();

    if(key_name == "label" || key_name == "namespace" || key_name == "type"){
        TypeKey::BindNamespaceAndLabelToType(this, true);
    }
}
