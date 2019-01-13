#include "vectorinst.h"
#include "../../entityfactorybroker.h"
#include "../../entityfactoryregistrybroker.h"
#include "aggregateinst.h"

const NODE_KIND node_kind = NODE_KIND::VECTOR_INST;
const QString kind_string = "Vector Instance";

void VectorInst::RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker){
    broker.RegisterWithEntityFactory(node_kind, kind_string, [](EntityFactoryBroker& broker, bool is_temp_node){
        return new VectorInst(broker, is_temp_node);
    });
}

VectorInst::VectorInst(EntityFactoryBroker& broker, bool is_temp) : DataNode(broker, node_kind, is_temp){
    addInstancesDefinitionKind(NODE_KIND::VECTOR);
    setChainableDefinition();

    setAcceptsNodeKind(NODE_KIND::ENUM_INST);
    setAcceptsNodeKind(NODE_KIND::AGGREGATE_INST);
    setAcceptsNodeKind(NODE_KIND::MEMBER_INST);

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

bool VectorInst::canAdoptChild(Node *child)
{
   
    if(getChildrenCount() > 0){
        return false;
    }
    return Node::canAdoptChild(child);
}

void VectorInst::parentSet(Node* parent){
    AggregateInst::ParentSet(this);
    DataNode::parentSet(parent);
}