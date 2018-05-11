#include "periodiceventinstance.h"
#include "../../entityfactorybroker.h"
#include "../../entityfactoryregistrybroker.h"

const NODE_KIND node_kind = NODE_KIND::PERIODICEVENT_INSTANCE;
const QString kind_string = "PeriodicEventInstance";

void PeriodicEventInstance::RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker){
    broker.RegisterWithEntityFactory(node_kind, kind_string, [](EntityFactoryBroker& broker, bool is_temp_node){
        return new PeriodicEventInstance(broker, is_temp_node);
        });
}

PeriodicEventInstance::PeriodicEventInstance(EntityFactoryBroker& broker, bool is_temp) : Node(broker, node_kind, is_temp){
    //Setup State
    addInstancesDefinitionKind(NODE_KIND::PERIODICEVENT);
    setAcceptsNodeKind(NODE_KIND::ATTRIBUTE_INSTANCE);
    
    if(is_temp){
        //Break out early for temporary entities
        return;
    }

    /*auto frequency = broker.ConstructChildNode(*this, NODE_KIND::ATTRIBUTE_INSTANCE);
    broker.AttachData(frequency, "label", QVariant::String, "Frequency", true);
    broker.AttachData(frequency, "icon", QVariant::String, "Variable", true);
    broker.AttachData(frequency, "icon_prefix", QVariant::String, "EntityIcons", true);*/
}

bool PeriodicEventInstance::canAdoptChild(Node* child)
{
    auto child_kind = child->getNodeKind();
    switch(child_kind){
        case NODE_KIND::ATTRIBUTE_INSTANCE:{
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