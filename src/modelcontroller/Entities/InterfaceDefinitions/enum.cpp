#include "enum.h"
#include "../data.h"
#include "../Keys/typekey.h"



Enum::Enum(EntityFactory* factory) : Node(factory, NODE_KIND::ENUM, "Enum"){
	auto node_kind = NODE_KIND::ENUM;
	QString kind_string = "Enum";
    RegisterNodeKind(factory, node_kind, kind_string, [](){return new Enum();});

    RegisterDefaultData(factory, node_kind, "namespace", QVariant::String, true);
    RegisterDefaultData(factory, node_kind, "type", QVariant::String, true);
};

Enum::Enum():Node(NODE_KIND::ENUM)
{
    setNodeType(NODE_TYPE::DEFINITION);
    setAcceptsEdgeKind(EDGE_KIND::DEFINITION);
    
    setInstanceKind(NODE_KIND::ENUM_INSTANCE);
    
}

bool Enum::canAdoptChild(Node* child)
{
    switch(child->getNodeKind()){
    case NODE_KIND::ENUM_MEMBER:
        break;
    default:
        return false;
    }
    return Node::canAdoptChild(child);
}

bool Enum::canAcceptEdge(EDGE_KIND, Node *)
{
    return false;
}

void Enum::DataAdded(Data* data){
    Node::DataAdded(data);

    auto key_name = data->getKeyName();

    if(key_name == "label" || key_name == "namespace" || key_name == "type"){
        TypeKey::BindNamespaceAndLabelToType(this, true);
    }
}


void Enum::childAdded(Node* child){
    Node::childAdded(child);
    TypeKey::BindTypes(this, child, true);
}

void Enum::childRemoved(Node* child){
    Node::childRemoved(child);
    TypeKey::BindTypes(this, child, false);
}