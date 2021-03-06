#include "variable.h"

#include "../../entityfactorybroker.h"
#include "../../entityfactoryregistrybroker.h"
#include "../../entityfactoryregistrybroker.h"
#include "../Keys/typekey.h"

const NODE_KIND node_kind = NODE_KIND::VARIABLE;
const QString kind_string = "Variable";

void Variable::RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker){
    broker.RegisterWithEntityFactory(node_kind, kind_string, [](EntityFactoryBroker& broker, bool is_temp_node){
        return new Variable(broker, is_temp_node);
        });
}

Variable::Variable(EntityFactoryBroker& broker, bool is_temp) : DataNode(broker, node_kind, is_temp){
    //Setup State
    setDataProducer(true);
    setDataReceiver(true);
    setAcceptsNodeKind(NODE_KIND::MEMBER);
    setAcceptsNodeKind(NODE_KIND::AGGREGATE_INST);
    setAcceptsNodeKind(NODE_KIND::VECTOR);
    setAcceptsNodeKind(NODE_KIND::ENUM_INST);
    setAcceptsNodeKind(NODE_KIND::EXTERNAL_TYPE);

    if(is_temp){
        //Break out early for temporary entities
        return;
    }
    
    //Setup Data
    broker.AttachData(this, KeyName::Index, QVariant::Int, ProtectedState::UNPROTECTED);
    broker.AttachData(this, KeyName::Row, QVariant::Int, ProtectedState::PROTECTED, 1);
    broker.AttachData(this, KeyName::Column, QVariant::Int, ProtectedState::PROTECTED, 1);
}

bool Variable::canAdoptChild(Node* child)
{
    if(getChildrenCount() > 0){
        return false;
    }

    return DataNode::canAdoptChild(child);
}

void Variable::childAdded(Node* child){
    DataNode::childAdded(child);
    TypeKey::BindTypes(child, this, true, true);
}

void Variable::childRemoved(Node* child){
    DataNode::childRemoved(child);
    TypeKey::BindTypes(child, this, true, false);
}