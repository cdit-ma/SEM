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
    setNodeType(NODE_TYPE::DEFINITION);
    setAcceptsEdgeKind(EDGE_KIND::DEFINITION);

    setInstanceKind(NODE_KIND::COMPONENT_INSTANCE);
    setImplKind(NODE_KIND::COMPONENT_IMPL);
}

bool Component::canAdoptChild(Node *child)
{
    switch(child->getNodeKind()){
    case NODE_KIND::ATTRIBUTE:
    case NODE_KIND::INEVENTPORT:
    case NODE_KIND::OUTEVENTPORT:
        break;
    default:
        return false;
    }
    return Node::canAdoptChild(child);
}

bool Component::canAcceptEdge(EDGE_KIND, Node *)
{
    return false;
}


void Component::DataAdded(Data* data){
    Node::DataAdded(data);

    auto key_name = data->getKeyName();

    if(key_name == "label" || key_name == "namespace" || key_name == "type"){
        TypeKey::BindNamespaceAndLabelToType(this, true);
    }
}
