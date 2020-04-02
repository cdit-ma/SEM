#include "replierportimpl.h"
#include "../../../entityfactorybroker.h"
#include "../../../entityfactoryregistrybroker.h"

const NODE_KIND node_kind = NODE_KIND::PORT_REPLIER_IMPL;
const QString kind_string = "Replier Port Impl";

void MEDEA::ReplierPortImpl::RegisterWithEntityFactory(::EntityFactoryRegistryBroker& broker){
    broker.RegisterWithEntityFactory(node_kind, kind_string, [](::EntityFactoryBroker& broker, bool is_temp_node){
        return new MEDEA::ReplierPortImpl(broker, is_temp_node);
        });
}

MEDEA::ReplierPortImpl::ReplierPortImpl(::EntityFactoryBroker& broker, bool is_temp) : Node(broker, node_kind, is_temp){
    setNodeType(NODE_TYPE::BEHAVIOUR_CONTAINER);
    addImplsDefinitionKind(NODE_KIND::PORT_REPLIER);

    setAcceptsNodeKind(NODE_KIND::INPUT_PARAMETER_GROUP_INST);
    setAcceptsNodeKind(NODE_KIND::RETURN_PARAMETER_GROUP_INST);
    
    for(auto node_kind : ContainerNode::getAcceptedNodeKinds()){
        setAcceptsNodeKind(node_kind);
    }

    if(is_temp){
        //Break out early for temporary entities
        return;
    }

    //Setup Data
    broker.AttachData(this, KeyName::Type, QVariant::String, ProtectedState::PROTECTED);
    broker.AttachData(this, KeyName::Index, QVariant::Int, ProtectedState::UNPROTECTED);
}

bool MEDEA::ReplierPortImpl::canAdoptChild(Node* child)
{
    auto child_node_kind = child->getNodeKind();

    switch(child_node_kind){
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

bool MEDEA::ReplierPortImpl::canAcceptEdge(EDGE_KIND edge_kind, Node * dst)
{
    if(canCurrentlyAcceptEdgeKind(edge_kind, dst) == false){
        return false;
    }

    switch(edge_kind){
    case EDGE_KIND::DEFINITION:{
        //Can only connect a definition edge to an Aggregate/AggregateInstance..
        if(dst->getNodeKind() != NODE_KIND::PORT_REPLIER){
            return false;
        }
        break;
    }
    default:
        break;
    }
    return Node::canAcceptEdge(edge_kind, dst);
}
