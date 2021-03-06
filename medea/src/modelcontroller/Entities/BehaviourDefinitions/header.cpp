#include "header.h"
#include "../../entityfactorybroker.h"
#include "../../entityfactoryregistrybroker.h"


const NODE_KIND node_kind = NODE_KIND::HEADER;
const QString kind_string = "Header";

void Header::RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker){
    broker.RegisterWithEntityFactory(node_kind, kind_string, [](EntityFactoryBroker& broker, bool is_temp_node){
        return new Header(broker, is_temp_node);
        });
}

Header::Header(EntityFactoryBroker& broker, bool is_temp) : Node(broker, node_kind, is_temp){
    if(is_temp){
        //Break out early for temporary entities
        return;
    }
    
    //Setup Data
    broker.AttachData(this, KeyName::Code, QVariant::String, ProtectedState::UNPROTECTED);
    
    auto header_location = broker.AttachData(this, KeyName::HeaderLocation, QVariant::String, ProtectedState::UNPROTECTED);
    header_location->addValidValues({"CPP", "Class Declaration", "Header"});

    broker.AttachData(this, KeyName::Index, QVariant::Int, ProtectedState::UNPROTECTED);
    broker.AttachData(this, KeyName::Row, QVariant::Int, ProtectedState::PROTECTED, 1);
    broker.AttachData(this, KeyName::Column, QVariant::Int, ProtectedState::PROTECTED, -1);
}