#include "externaltype.h"
#include "containernode.h"
#include "../Keys/typekey.h"
#include "../../entityfactory.h"
#include <QDebug>

const NODE_KIND node_kind = NODE_KIND::EXTERNAL_TYPE;
const QString kind_string = "ExternalType";

void MEDEA::ExternalType::RegisterWithEntityFactory(EntityFactory& factory){
    Node::RegisterWithEntityFactory(factory, node_kind, kind_string, [](EntityFactory& factory, bool is_temp_node){
        return new MEDEA::ExternalType(factory, is_temp_node);
        });
}

MEDEA::ExternalType::ExternalType(EntityFactory& factory, bool is_temp) : Node(factory, node_kind, is_temp){
    if(is_temp){
        return;
    }

    //SetupState
    setChainableDefinition();

    //Setup Data
    factory.AttachData(this, "type", QVariant::String, "", true);
    factory.AttachData(this, "label", QVariant::String, "", true);
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
                for(auto child : container->getChildrenOfKind(NODE_KIND::CLASS_INSTANCE)){
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