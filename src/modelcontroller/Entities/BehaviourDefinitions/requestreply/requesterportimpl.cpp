#include "requesterportimpl.h"
#include "../containernode.h"
#include "../../../entityfactorybroker.h"
#include "../../../entityfactoryregistrybroker.h"

const NODE_KIND node_kind = NODE_KIND::PORT_REQUESTER_IMPL;
const QString kind_string = "Requester Port Impl";

void MEDEA::RequesterPortImpl::RegisterWithEntityFactory(::EntityFactoryRegistryBroker& broker){
    broker.RegisterWithEntityFactory(node_kind, kind_string, [](::EntityFactoryBroker& broker, bool is_temp_node){
        return new MEDEA::RequesterPortImpl(broker, is_temp_node);
        });
}

MEDEA::RequesterPortImpl::RequesterPortImpl(::EntityFactoryBroker& broker, bool is_temp) : Node(broker, node_kind, is_temp){
    //Setup State
    addImplsDefinitionKind(NODE_KIND::PORT_REQUESTER);
    setNodeType(NODE_TYPE::BEHAVIOUR_ELEMENT);

    //Allow links from within things like SubscriberPortImpls back to the
    SetEdgeRuleActive(Node::EdgeRule::ALWAYS_CHECK_VALID_DEFINITIONS);
    SetEdgeRuleActive(Node::EdgeRule::ALLOW_MULTIPLE_IMPLEMENTATIONS);

    setAcceptsNodeKind(NODE_KIND::INPUT_PARAMETER);
    setAcceptsNodeKind(NODE_KIND::RETURN_PARAMETER);
    setAcceptsNodeKind(NODE_KIND::INPUT_PARAMETER_GROUP_INST);
    setAcceptsNodeKind(NODE_KIND::RETURN_PARAMETER_GROUP_INST);

    if(is_temp){
        //Break out early for temporary entities
        return;
    }
    
    //Setup Data
    broker.AttachData(this, "type", QVariant::String, ProtectedState::PROTECTED);
    broker.AttachData(this, "index", QVariant::Int, ProtectedState::UNPROTECTED);

    auto timeout_param = broker.ConstructChildNode(*this, NODE_KIND::INPUT_PARAMETER);
    broker.AttachData(timeout_param, "label", QVariant::String, ProtectedState::PROTECTED, "Timeout (MS)");
    broker.AttachData(timeout_param, "inner_type", QVariant::String, ProtectedState::PROTECTED, "Integer");
    broker.AttachData(timeout_param, "row", QVariant::Int, ProtectedState::PROTECTED, 0);
    broker.AttachData(timeout_param, "column", QVariant::Int, ProtectedState::PROTECTED, -1);
    broker.AttachData(timeout_param, "index", QVariant::Int, ProtectedState::PROTECTED);
    broker.AttachData(timeout_param, "icon_prefix", QVariant::String, ProtectedState::PROTECTED, "Icons");
    broker.AttachData(timeout_param, "icon", QVariant::String, ProtectedState::PROTECTED, "clockAlarm");
    broker.AttachData(timeout_param, "value", QVariant::String, ProtectedState::UNPROTECTED, 1000);


    auto success_param = broker.ConstructChildNode(*this, NODE_KIND::RETURN_PARAMETER);
    broker.AttachData(success_param, "label", QVariant::String, ProtectedState::PROTECTED, "Success");
    broker.AttachData(success_param, "inner_type", QVariant::String, ProtectedState::PROTECTED, "Boolean");
    broker.AttachData(success_param, "row", QVariant::Int, ProtectedState::PROTECTED, 0);
    broker.AttachData(success_param, "column", QVariant::Int, ProtectedState::PROTECTED, 1);
    broker.AttachData(success_param, "index", QVariant::Int, ProtectedState::PROTECTED);
    broker.AttachData(success_param, "icon_prefix", QVariant::String, ProtectedState::PROTECTED, "Icons");
    broker.AttachData(success_param, "icon", QVariant::String, ProtectedState::PROTECTED, "circleTickDark");

}


bool MEDEA::RequesterPortImpl::canAdoptChild(Node* child)
{
   auto child_node_kind = child->getNodeKind();

    switch(child_node_kind){
        case NODE_KIND::INPUT_PARAMETER:
        case NODE_KIND::RETURN_PARAMETER:
        case NODE_KIND::INPUT_PARAMETER_GROUP_INST:
        case NODE_KIND::RETURN_PARAMETER_GROUP_INST:{
            if(getChildrenOfKindCount(child_node_kind) > 0){
                return false;
            }
            break;
        }
        default:
            break;
    }
    return Node::canAdoptChild(child);
}

bool MEDEA::RequesterPortImpl::canAcceptEdge(EDGE_KIND edge_kind, Node * dst)
{
    if(canCurrentlyAcceptEdgeKind(edge_kind, dst) == false){
        return false;
    }

    switch(edge_kind){
    case EDGE_KIND::DEFINITION:{
        //Can only connect a definition edge to an Aggregate/AggregateInstance..
        if(dst->getNodeKind() != NODE_KIND::PORT_REQUESTER){
            return false;
        }
        break;
    }
    default:
        break;
    }
    return Node::canAcceptEdge(edge_kind, dst);
}

QSet<Node*> MEDEA::RequesterPortImpl::getParentNodesForValidDefinition(){
    QSet<Node*> parents;
    //Need to look at The Component's defintion
    auto component = getTopBehaviourContainer();
    if(component){
        auto component_definition = component->getDefinition(true);
        if(component_definition){
            parents << component_definition;
        }
    }
    return parents;
}

Node* MEDEA::RequesterPortImpl::getTopBehaviourContainer(){
    if(!top_behaviour_calculated){
        top_behaviour_container = ContainerNode::getTopBehaviourContainer(this);
        top_behaviour_calculated = true;
    }
    return top_behaviour_container;
}