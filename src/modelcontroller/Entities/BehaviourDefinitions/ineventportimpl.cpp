#include "ineventportimpl.h"
#include "../../entityfactorybroker.h"
#include "../../entityfactoryregistrybroker.h"

const NODE_KIND node_kind = NODE_KIND::INEVENTPORT_IMPL;
const QString kind_string = "In EventPort Impl";

void InEventPortImpl::RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker){
    broker.RegisterWithEntityFactory(node_kind, kind_string, [](EntityFactoryBroker& broker, bool is_temp_node){
        return new InEventPortImpl(broker, is_temp_node);
        });
}

InEventPortImpl::InEventPortImpl(EntityFactoryBroker& broker, bool is_temp) : Node(broker, node_kind, is_temp){
    //Setup State
    setNodeType(NODE_TYPE::BEHAVIOUR_CONTAINER);
    addImplsDefinitionKind(NODE_KIND::INEVENTPORT);
    setAcceptsNodeKind(NODE_KIND::AGGREGATE_INSTANCE);
    
    for(auto node_kind : ContainerNode::getAcceptedNodeKinds()){
        setAcceptsNodeKind(node_kind);
    }

    if(is_temp){
        //Break out early for temporary entities
        return;
    }

    //Setup Data
    broker.AttachData(this, "type", QVariant::String, ProtectedState::PROTECTED);
    broker.AttachData(this, "index", QVariant::Int, ProtectedState::UNPROTECTED);
}

bool InEventPortImpl::canAdoptChild(Node *child)
{
    auto child_kind = child->getNodeKind();
    
    switch(child_kind){
        case NODE_KIND::AGGREGATE_INSTANCE:{
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
