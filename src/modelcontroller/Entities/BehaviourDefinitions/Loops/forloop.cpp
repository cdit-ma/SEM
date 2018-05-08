#include "forloop.h"
#include "../containernode.h"
#include "../../../entityfactory.h"

const NODE_KIND node_kind = NODE_KIND::FOR_LOOP;
const QString kind_string = "ForLoop";

void MEDEA::ForLoop::RegisterWithEntityFactory(EntityFactory& factory){
    Node::RegisterWithEntityFactory(factory, node_kind, kind_string, [](EntityFactory& factory, bool is_temp_node){
        return new MEDEA::ForLoop(factory, is_temp_node);
        });
}

MEDEA::ForLoop::ForLoop(EntityFactory& factory, bool is_temp) : Node(factory, node_kind, is_temp){
    if(is_temp){
        return;
    }

    //SetupState
    setNodeType(NODE_TYPE::BEHAVIOUR_ELEMENT);
    setNodeType(NODE_TYPE::BEHAVIOUR_CONTAINER);

    setAcceptsNodeKind(NODE_KIND::VARIABLE_PARAMETER);
    setAcceptsNodeKind(NODE_KIND::INPUT_PARAMETER);

    for(auto node_kind : ContainerNode::getAcceptedNodeKinds()){
        setAcceptsNodeKind(node_kind);
    }
    setLabelFunctional(false);

    factory.AttachData(this, "label", QVariant::String, "for", true);
}

bool MEDEA::ForLoop::canAdoptChild(Node *child)
{
    auto child_kind = child->getNodeKind();
    
    switch(child_kind){
        case NODE_KIND::VARIABLE_PARAMETER:{
            if(getChildrenOfKindCount(child_kind) > 0){
                return false;
            }

        }
        case NODE_KIND::INPUT_PARAMETER:
            if(getChildrenOfKindCount(child_kind) >= 2){
                return false;
            }
            break;
    default:
        break;
    }
    
    //Ignore the can adopt child from condition
    return Node::canAdoptChild(child);
}
