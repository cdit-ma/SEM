#include "enum.h"
#include "../data.h"
#include "../Keys/typekey.h"
#include "../../entityfactory.h"

const NODE_KIND node_kind = NODE_KIND::ENUM;
const QString kind_string = "Enum";

void Enum::RegisterWithEntityFactory(EntityFactory& factory){
    Node::RegisterWithEntityFactory(factory, node_kind, kind_string, [](EntityFactory& factory, bool is_temp_node){
        return new Enum(factory, is_temp_node);
    });
}

Enum::Enum(EntityFactory& factory, bool is_temp) : Node(factory, node_kind, is_temp){
    if(is_temp){
        return;
    }

    //Setup State
    addInstanceKind(NODE_KIND::ENUM_INSTANCE);
    setAcceptsNodeKind(NODE_KIND::ENUM_MEMBER);

    //Setup Data
    factory.AttachData(this, "namespace", QVariant::String, "", true);
    factory.AttachData(this, "type", QVariant::String, "", true);
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