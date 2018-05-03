#include "externaltype.h"
#include "containernode.h"
#include "../edge.h"
#include "../data.h"
#include "../Keys/typekey.h"
#include <QDebug>

const NODE_KIND node_kind = NODE_KIND::EXTERNAL_TYPE;
const QString kind_string = "ExternalType";

MEDEA::ExternalType::ExternalType(EntityFactory* factory) : Node(factory, node_kind, kind_string){
	RegisterNodeKind(factory, node_kind, kind_string, [](){return new ExternalType();});

    //Register DefaultData
    RegisterDefaultData(factory, node_kind, "type", QVariant::String, true);
    RegisterDefaultData(factory, node_kind, "label", QVariant::String, true);
};

MEDEA::ExternalType::ExternalType() : Node(node_kind)
{
    setChainableDefinition();
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