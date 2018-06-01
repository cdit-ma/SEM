#include "periodicevent.h"
#include "containernode.h"
#include "../../entityfactorybroker.h"
#include "../../entityfactoryregistrybroker.h"

const NODE_KIND node_kind = NODE_KIND::PERIODICEVENT;
const QString kind_string = "Periodic Event";

void PeriodicEvent::RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker){
    broker.RegisterWithEntityFactory(node_kind, kind_string, [](EntityFactoryBroker& broker, bool is_temp_node){
        return new PeriodicEvent(broker, is_temp_node);
        });
}

PeriodicEvent::PeriodicEvent(EntityFactoryBroker& broker, bool is_temp) : Node(broker, node_kind, is_temp){
    //Setup State
    setNodeType(NODE_TYPE::BEHAVIOUR_CONTAINER);
    addInstanceKind(NODE_KIND::PERIODICEVENT_INSTANCE);
    setAcceptsNodeKind(NODE_KIND::ATTRIBUTE);
    
    for(auto node_kind : ContainerNode::getAcceptedNodeKinds()){
        setAcceptsNodeKind(node_kind);
    }
    
    if(is_temp){
        //Break out early for temporary entities
        return;
    }

    auto frequency = broker.ConstructChildNode(*this, NODE_KIND::ATTRIBUTE);
    broker.AttachData(frequency, "icon_prefix", QVariant::String, ProtectedState::PROTECTED, "Icons");
    broker.AttachData(frequency, "icon", QVariant::String, ProtectedState::PROTECTED, "timeElapsed");
    broker.AttachData(frequency, "label", QVariant::String, ProtectedState::PROTECTED, "Frequency");
    broker.AttachData(frequency, "type", QVariant::String, ProtectedState::PROTECTED, "Double");
    

    broker.AttachData(this, "index", QVariant::Int, ProtectedState::UNPROTECTED);
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