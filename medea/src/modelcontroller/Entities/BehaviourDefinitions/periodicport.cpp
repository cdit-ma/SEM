#include "periodicport.h"
#include "containernode.h"
#include "../../entityfactorybroker.h"
#include "../../entityfactoryregistrybroker.h"
#include "../InterfaceDefinitions/datanode.h"

const NODE_KIND node_kind = NODE_KIND::PORT_PERIODIC;
const QString kind_string = "Periodic Port";

void PeriodicEvent::RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker){
    broker.RegisterWithEntityFactory(node_kind, kind_string, [](EntityFactoryBroker& broker, bool is_temp_node){
        return new PeriodicEvent(broker, is_temp_node);
        });
}

PeriodicEvent::PeriodicEvent(EntityFactoryBroker& broker, bool is_temp) : Node(broker, node_kind, is_temp){
    //Setup State
    setNodeType(NODE_TYPE::BEHAVIOUR_CONTAINER);
    addInstanceKind(NODE_KIND::PORT_PERIODIC_INST);
    setAcceptsNodeKind(NODE_KIND::ATTRIBUTE);
    
    for(auto node_kind : ContainerNode::getAcceptedNodeKinds()){
        setAcceptsNodeKind(node_kind);
    }
    
    if(is_temp){
        //Break out early for temporary entities
        return;
    }

    auto frequency = (DataNode*)broker.ConstructChildNode(*this, NODE_KIND::ATTRIBUTE);
    broker.AttachData(frequency, KeyName::IconPrefix, QVariant::String, ProtectedState::PROTECTED, "Icons");
    broker.AttachData(frequency, KeyName::Icon, QVariant::String, ProtectedState::PROTECTED, "timeElapsedTwoTone");
    broker.AttachData(frequency, KeyName::Label, QVariant::String, ProtectedState::PROTECTED, "Frequency");
    broker.AttachData(frequency, KeyName::Type, QVariant::String, ProtectedState::PROTECTED, "Double");
    broker.AttachData(this, KeyName::Index, QVariant::Int, ProtectedState::UNPROTECTED);

    frequency->setGlobalScopedDataLinker(true);
}

bool PeriodicEvent::canAdoptChild(Node* child)
{
    auto child_kind = child->getNodeKind();
    switch(child_kind){
        case NODE_KIND::ATTRIBUTE:{
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
