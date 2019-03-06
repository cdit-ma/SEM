#include "vectorinstance.h"
#include "../../entityfactorybroker.h"
#include "../../entityfactoryregistrybroker.h"
#include "aggregateinstance.h"

const NODE_KIND node_kind = NODE_KIND::VECTOR_INSTANCE;
const QString kind_string = "Vector Instance";

void VectorInstance::RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker){
    broker.RegisterWithEntityFactory(node_kind, kind_string, [](EntityFactoryBroker& broker, bool is_temp_node){
        return new VectorInstance(broker, is_temp_node);
    });
}

VectorInstance::VectorInstance(EntityFactoryBroker& broker, bool is_temp) : DataNode(broker, node_kind, is_temp){
    //Setup State
    //setDataProducer(true);
    //setDataReceiver(true);

    addInstancesDefinitionKind(NODE_KIND::VECTOR);
    setChainableDefinition();

    setAcceptsNodeKind(NODE_KIND::ENUM_INSTANCE);
    setAcceptsNodeKind(NODE_KIND::AGGREGATE_INSTANCE);
    setAcceptsNodeKind(NODE_KIND::MEMBER_INSTANCE);

    if(is_temp){
        //Break out early for temporary entities
        return;
    }

    //Setup Data
    broker.AttachData(this, "label", QVariant::String, ProtectedState::PROTECTED);
    broker.AttachData(this, "outer_type", QVariant::String, ProtectedState::PROTECTED, "Vector");
    broker.AttachData(this, "icon", QVariant::String, ProtectedState::PROTECTED);
    broker.AttachData(this, "icon_prefix", QVariant::String, ProtectedState::PROTECTED);
}

bool VectorInstance::canAdoptChild(Node *child)
{
   
    if(getChildrenCount() > 0){
        return false;
    }
    return Node::canAdoptChild(child);
}

void VectorInstance::parentSet(Node* parent){
    AggregateInstance::ParentSet(this);
    DataNode::parentSet(parent);
}