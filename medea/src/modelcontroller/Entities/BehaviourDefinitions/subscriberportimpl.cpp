#include "subscriberportimpl.h"
#include "../../entityfactorybroker.h"
#include "../../entityfactoryregistrybroker.h"

const NODE_KIND node_kind = NODE_KIND::PORT_SUBSCRIBER_IMPL;
const QString kind_string = "Subscriber Port Impl";

void SubscriberPortImpl::RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker){
    broker.RegisterWithEntityFactory(node_kind, kind_string, [](EntityFactoryBroker& broker, bool is_temp_node){
        return new SubscriberPortImpl(broker, is_temp_node);
        });
}

SubscriberPortImpl::SubscriberPortImpl(EntityFactoryBroker& broker, bool is_temp) : Node(broker, node_kind, is_temp){
    //Setup State
    setNodeType(NODE_TYPE::BEHAVIOUR_CONTAINER);
    addImplsDefinitionKind(NODE_KIND::PORT_SUBSCRIBER);
    setAcceptsNodeKind(NODE_KIND::AGGREGATE_INST);
    
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

bool SubscriberPortImpl::canAdoptChild(Node *child)
{
    auto child_kind = child->getNodeKind();
    
    switch(child_kind){
        case NODE_KIND::AGGREGATE_INST:{
            if(getChildrenOfKindCount(child_kind) > 0){
                return false;
            }
            break;
        default:
            break;
        }
    }
    return Node::canAdoptChild(child);
}
