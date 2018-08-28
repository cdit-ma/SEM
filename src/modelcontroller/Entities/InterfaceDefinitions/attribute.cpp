#include "attribute.h"
#include "../../entityfactorybroker.h"
#include "../../entityfactoryregistrybroker.h"
#include "../../entityfactoryregistrybroker.h"
#include "../Keys/typekey.h"

const NODE_KIND node_kind = NODE_KIND::ATTRIBUTE;
const QString kind_string = "Attribute";

void Attribute::RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker){
    broker.RegisterWithEntityFactory(node_kind, kind_string, [](EntityFactoryBroker& broker, bool is_temp_node){
        return new Attribute(broker, is_temp_node);
    });
}

Attribute::Attribute(EntityFactoryBroker& broker, bool is_temp) : DataNode(broker, node_kind, is_temp, false){
    //Setup State
    addInstanceKind(NODE_KIND::ATTRIBUTE_INSTANCE);
    addImplKind(NODE_KIND::ATTRIBUTE_IMPL);

    if(is_temp){
        //Break out early for temporary entities
        return;
    }


    //Setup Data
    auto type_data = broker.AttachData(this, "type", QVariant::String, ProtectedState::UNPROTECTED, TypeKey::GetDefaultPrimitiveType());
    type_data->addValidValues(TypeKey::GetValidPrimitiveTypes());
    
    broker.AttachData(this, "index", QVariant::Int, ProtectedState::UNPROTECTED);
    broker.AttachData(this, "row", QVariant::Int, ProtectedState::PROTECTED, 1);
}

void Attribute::parentSet(Node* parent){
    if(getViewAspect() != VIEW_ASPECT::INTERFACES){
        auto parent_kind = parent->getNodeKind();

        switch(parent_kind){
            case NODE_KIND::PORT_PERIODIC:{
                break;
            }
            default:{
                setDataProducer(true);
                break;
            }
        }
    }
    DataNode::parentSet(parent);
}