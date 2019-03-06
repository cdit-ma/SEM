#include "externaltype.h"
#include "containernode.h"
#include "../Keys/typekey.h"
#include "../../entityfactorybroker.h"
#include "../../entityfactoryregistrybroker.h"
#include <QDebug>

const NODE_KIND node_kind = NODE_KIND::EXTERNAL_TYPE;
const QString kind_string = "External Type";

void MEDEA::ExternalType::RegisterWithEntityFactory(::EntityFactoryRegistryBroker& broker){
    broker.RegisterWithEntityFactory(node_kind, kind_string, [](::EntityFactoryBroker& broker, bool is_temp_node){
        return new MEDEA::ExternalType(broker, is_temp_node);
        });
}

MEDEA::ExternalType::ExternalType(::EntityFactoryBroker& broker, bool is_temp) : Node(broker, node_kind, is_temp){
    //SetupState
    setChainableDefinition();

    if(is_temp){
        //Break out early for temporary entities
        return;
    }

    //Setup Data
    broker.AttachData(this, "type", QVariant::String, ProtectedState::PROTECTED);
    broker.AttachData(this, "inner_type", QVariant::String, ProtectedState::PROTECTED);
    broker.AttachData(this, "outer_type", QVariant::String, ProtectedState::PROTECTED);
    broker.AttachData(this, "label", QVariant::String, ProtectedState::PROTECTED);
}

bool MEDEA::ExternalType::canAcceptEdge(EDGE_KIND edge_kind, Node *dst)
{
    if(canCurrentlyAcceptEdgeKind(edge_kind, dst) == false){
        return false;
    }

    switch(edge_kind){
        case EDGE_KIND::DEFINITION:{
            if(dst->getNodeKind() != NODE_KIND::EXTERNAL_TYPE){
                return false;
            }

            auto container = getTopBehaviourContainer();
            auto dst_parent = dst->getParentNode();

            if(dst_parent->getNodeKind() != NODE_KIND::CLASS){
                return false;
            }


            bool got_instance = false;
            if(container){
                for(auto child : container->getChildrenOfKind(NODE_KIND::CLASS_INST)){
                    if(child->getDefinition(true) == dst_parent){
                        got_instance = true;
                        break;
                    }
                }
            }

            if(!got_instance){
                return false;
            }
            return true;

            break;
        }
        default:
            break;
    }

    return Node::canAcceptEdge(edge_kind, dst);
}

Node* MEDEA::ExternalType::getTopBehaviourContainer(){
    if(!top_behaviour_calculated){
        top_behaviour_container = ContainerNode::getTopBehaviourContainer(this);
        top_behaviour_calculated = true;
    }
    return top_behaviour_container;
}