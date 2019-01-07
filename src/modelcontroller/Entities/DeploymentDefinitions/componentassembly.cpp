#include "componentassembly.h"
#include "../../entityfactorybroker.h"
#include "../../entityfactoryregistrybroker.h"
#include "../../entityfactoryregistrybroker.h"
const NODE_KIND node_kind = NODE_KIND::COMPONENT_ASSEMBLY;
const QString kind_string = "Component Assembly";

void ComponentAssembly::RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker){
    broker.RegisterWithEntityFactory(node_kind, kind_string, [](EntityFactoryBroker& broker, bool is_temp_node){
        return new ComponentAssembly(broker, is_temp_node);
        });
}

ComponentAssembly::ComponentAssembly(EntityFactoryBroker& broker, bool is_temp) : Node(broker, node_kind, is_temp){
    setAcceptsEdgeKind(EDGE_KIND::DEPLOYMENT, EDGE_DIRECTION::SOURCE);
    setAcceptsEdgeKind(EDGE_KIND::QOS, EDGE_DIRECTION::SOURCE);

    setAcceptsNodeKind(NODE_KIND::COMPONENT_ASSEMBLY);
    setAcceptsNodeKind(NODE_KIND::VARIABLE);
    setAcceptsNodeKind(NODE_KIND::COMPONENT_INSTANCE);
    setAcceptsNodeKind(NODE_KIND::PORT_SUBSCRIBER_DELEGATE);
    setAcceptsNodeKind(NODE_KIND::PORT_PUBLISHER_DELEGATE);
    setAcceptsNodeKind(NODE_KIND::DEPLOYMENT_ATTRIBUTE);

    if(is_temp){
        //Break out early for temporary entities
        return;
    }

    auto frequency = broker.ConstructChildNode(*this, NODE_KIND::VARIABLE);
    frequency->setLabelFunctional(false);
    broker.AttachData(frequency, "icon_prefix", QVariant::String, ProtectedState::PROTECTED, "Icons");
    broker.AttachData(frequency, "icon", QVariant::String, ProtectedState::PROTECTED, "copyX");
    broker.AttachData(frequency, "label", QVariant::String, ProtectedState::PROTECTED, "Replication");
    broker.AttachData(frequency, "inner_type", QVariant::String, ProtectedState::PROTECTED, "Integer");
    auto value_data = frequency->getData("value");


    broker.AttachData(this, "comment", QVariant::String, ProtectedState::UNPROTECTED);
    auto replicate_data = broker.AttachData(this, "replicate_value", QVariant::String, ProtectedState::PROTECTED, 1);
    value_data->linkData(replicate_data, true);
}


bool ComponentAssembly::canAdoptChild(Node* child)
{
    auto child_kind = child->getNodeKind();
    switch(child_kind){
        case NODE_KIND::VARIABLE:{
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