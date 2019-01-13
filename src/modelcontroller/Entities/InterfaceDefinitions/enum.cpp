#include "enum.h"
#include "../data.h"
#include "../Keys/typekey.h"
#include "../../entityfactorybroker.h"
#include "../../entityfactoryregistrybroker.h"

const NODE_KIND node_kind = NODE_KIND::ENUM;
const QString kind_string = "Enum";

void Enum::RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker){
    broker.RegisterWithEntityFactory(node_kind, kind_string, [](EntityFactoryBroker& broker, bool is_temp_node){
        return new Enum(broker, is_temp_node);
    });
}

Enum::Enum(EntityFactoryBroker& broker, bool is_temp) : Node(broker, node_kind, is_temp){
    
    //Setup State
    addInstanceKind(NODE_KIND::ENUM_INST);
    setAcceptsNodeKind(NODE_KIND::ENUM_MEMBER);

    if(is_temp){
        //Break out early for temporary entities
        return;
    }



    //Setup Data
    broker.AttachData(this, "namespace", QVariant::String, ProtectedState::PROTECTED);
    broker.AttachData(this, "type", QVariant::String, ProtectedState::PROTECTED);
    broker.AttachData(this, "column_count", QVariant::Int, ProtectedState::UNPROTECTED, 1);
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
    TypeKey::BindTypes(this, child, false, true);
}

void Enum::childRemoved(Node* child){
    Node::childRemoved(child);
    TypeKey::BindTypes(this, child, false, false);
}