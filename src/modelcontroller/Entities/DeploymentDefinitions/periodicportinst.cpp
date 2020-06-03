#include "periodicportinst.h"
#include "../../entityfactorybroker.h"
#include "../../entityfactoryregistrybroker.h"

const NODE_KIND node_kind = NODE_KIND::PORT_PERIODIC_INST;
const QString kind_string = "Periodic Port Instance";

void PeriodicPortInst::RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker){
    broker.RegisterWithEntityFactory(node_kind, kind_string, [](EntityFactoryBroker& broker, bool is_temp_node){
        return new PeriodicPortInst(broker, is_temp_node);
    });
}

PeriodicPortInst::PeriodicPortInst(EntityFactoryBroker& broker, bool is_temp) : Node(broker, node_kind, is_temp){
    //Setup State
    addInstancesDefinitionKind(NODE_KIND::PORT_PERIODIC);
    setAcceptsNodeKind(NODE_KIND::ATTRIBUTE_INST);
    
    if(is_temp){
        //Break out early for temporary entities
        return;
    }
    
    broker.AttachData(this, KeyName::Index, QVariant::Int, ProtectedState::UNPROTECTED);
    broker.AttachData(this, KeyName::Row, QVariant::Int, ProtectedState::UNPROTECTED, 0);
    broker.AttachData(this, KeyName::Column, QVariant::Int, ProtectedState::PROTECTED, 2);

    auto frequency = broker.ConstructChildNode(*this, NODE_KIND::ATTRIBUTE_INST);
    broker.AttachData(frequency, "type", QVariant::String, ProtectedState::PROTECTED, "Double");
    broker.AttachData(frequency, "value", QVariant::String, ProtectedState::UNPROTECTED, 1);
}

bool PeriodicPortInst::canAdoptChild(Node* child)
{
    auto child_kind = child->getNodeKind();
    switch(child_kind){
        case NODE_KIND::ATTRIBUTE_INST:{
            //Only allow one AttributeInstance
            if(getChildrenOfKindCount(child_kind) > 0){
                return false;
            }
            break;
        }
        default:
            break;
    }
    return Node::canAdoptChild(child);
}