#include "member.h"
#include "aggregateinstance.h"
#include "../../entityfactorybroker.h"
#include "../../entityfactoryregistrybroker.h"
#include "../../entityfactoryregistrybroker.h"
#include "../Keys/typekey.h"

const NODE_KIND node_kind = NODE_KIND::MEMBER;
const QString kind_string = "Member";

void Member::RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker){
    broker.RegisterWithEntityFactory(node_kind, kind_string, [](EntityFactoryBroker& broker, bool is_temp_node){
        return new Member(broker, is_temp_node);
    });
}

Member::Member(EntityFactoryBroker& broker, bool is_temp) : DataNode(broker, node_kind, is_temp, false){
    //Setup State
    addInstanceKind(NODE_KIND::MEMBER_INSTANCE);

    if(is_temp){
        //Break out early for temporary entities
        return;
    }

    //Setup Data
    broker.AttachData(this, "index", QVariant::Int, -1, false);
    auto type_data = broker.AttachData(this, "type", QVariant::String, "String", false);
    type_data->addValidValues(TypeKey::GetValidPrimitiveTypes());
    broker.AttachData(this, "key", QVariant::Bool, false, false);
}


void Member::parentSet(Node* parent){
    if(parent->getNodeKind() != NODE_KIND::VARIABLE){
        AggregateInstance::ParentSet(this);
    }
    DataNode::parentSet(parent);
}
