#include "forloop.h"

const NODE_KIND node_kind = NODE_KIND::FOR_LOOP;
const QString kind_string = "ForLoop";


MEDEA::ForLoop::ForLoop(EntityFactory* factory) : Node(factory, node_kind, kind_string){
	RegisterNodeKind(factory, node_kind, kind_string, [](){return new ForLoop();});
    RegisterDefaultData(factory, node_kind, "label", QVariant::String, false, "for");
};

MEDEA::ForLoop::ForLoop():Node(node_kind){
    setNodeType(NODE_TYPE::BEHAVIOUR_CONTAINER);
}

bool MEDEA::ForLoop::canAdoptChild(Node *child)
{
    auto condition_can_adopt = ContainerNode::canAdoptChild(child);

    if(!condition_can_adopt){
        auto child_kind = child->getNodeKind();
        
        switch(child_kind){
            case NODE_KIND::VARIABLE_PARAMETER:{
                if(getChildrenOfKind(child_kind, 0).size() >= 1){
                    return false;
                }

            }
            case NODE_KIND::INPUT_PARAMETER:
                if(getChildrenOfKind(child_kind, 0).size() >= 2){
                    return false;
                }
                break;
        default:
            return false;
        }
    }
    
    //Ignore the can adopt child from condition
    return Node::canAdoptChild(child);


    if(ContainerNode::canAdoptChild(child)){
        return Node::canAdoptChild(child);
    }
    return false;
}

bool MEDEA::ForLoop::canAcceptEdge(EDGE_KIND edgeKind, Node *dst)
{
    return Node::canAcceptEdge(edgeKind, dst);
}
  

