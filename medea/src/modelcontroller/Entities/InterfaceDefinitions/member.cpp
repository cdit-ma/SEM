#include "member.h"
#include "aggregateinst.h"
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
    addInstanceKind(NODE_KIND::MEMBER_INST);

    if(is_temp){
        //Break out early for temporary entities
        return;
    }

    //Setup Data
    setLabelFunctional(false);

    broker.AttachData(this, KeyName::Index, QVariant::Int, ProtectedState::UNPROTECTED);
    broker.AttachData(this, KeyName::PrimitiveType, QVariant::String, ProtectedState::UNPROTECTED);

    auto type_data = broker.AttachData(this, KeyName::Type, QVariant::String, ProtectedState::UNPROTECTED, TypeKey::GetDefaultPrimitiveType());
    type_data->addValidValues(TypeKey::GetValidPrimitiveTypes());
    
    broker.AttachData(this, KeyName::Key, QVariant::Bool, ProtectedState::UNPROTECTED, false);
}


void Member::parentSet(Node* parent)
{
    if (parent->getNodeKind() == NODE_KIND::AGGREGATE) {
        setLabelFunctional(true);
    }
    if (parent->getNodeKind() != NODE_KIND::VARIABLE) {
        AggregateInst::ParentSet(this);
    } else {
        getFactoryBroker().AttachData(this, KeyName::Label, QVariant::String, ProtectedState::PROTECTED);
    }
    DataNode::parentSet(parent);
}
