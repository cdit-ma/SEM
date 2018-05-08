#include "periodicevent.h"
#include "containernode.h"
#include "../../entityfactory.h"

const NODE_KIND node_kind = NODE_KIND::PERIODICEVENT;
const QString kind_string = "PeriodicEvent";

void PeriodicEvent::RegisterWithEntityFactory(EntityFactory& factory){
    Node::RegisterWithEntityFactory(factory, node_kind, kind_string, [](EntityFactory& factory, bool is_temp_node){
        return new PeriodicEvent(factory, is_temp_node);
        });
}

PeriodicEvent::PeriodicEvent(EntityFactory& factory, bool is_temp) : Node(factory, node_kind, is_temp){
    if(is_temp){
        return;
    }

    //Setup State
    setNodeType(NODE_TYPE::BEHAVIOUR_CONTAINER);
    setChainableDefinition();

    setAcceptsNodeKind(NODE_KIND::ATTRIBUTE_INSTANCE);
    
    for(auto node_kind : ContainerNode::getAcceptedNodeKinds()){
        setAcceptsNodeKind(node_kind);
    }
    //TODO: Do Special Adoption mate
}

bool PeriodicEvent::canAdoptChild(Node* child)
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

bool PeriodicEvent::canAcceptEdge(EDGE_KIND edge_kind, Node *dst)
{
    switch(edge_kind){
    case EDGE_KIND::DEFINITION:{
        switch(dst->getNodeKind()){
            case NODE_KIND::PERIODICEVENT:{
                if(getViewAspect() == VIEW_ASPECT::ASSEMBLIES){
                    if(dst->getViewAspect() != VIEW_ASPECT::BEHAVIOUR) {
                        return false;
                    }
                }else{
                    return false;
                }
                break;
            }
            default:
                return false;
        }
    }
    default:
        break;
    }

    return Node::canAcceptEdge(edge_kind, dst);
}
