#include "whileloop.h"

const NODE_KIND node_kind = NODE_KIND::WHILE_LOOP;
const QString kind_string = "WhileLoop";

MEDEA::WhileLoop::WhileLoop(EntityFactory* factory) : Node(factory, node_kind, kind_string){
	RegisterNodeKind(factory, node_kind, kind_string, [](){return new WhileLoop();});
    RegisterDefaultData(factory, node_kind, "label", QVariant::String, false, "while");
};

MEDEA::WhileLoop::WhileLoop():Node(NODE_KIND::WHILE_LOOP){
    setNodeType(NODE_TYPE::BEHAVIOUR_CONTAINER);

    setAcceptsNodeKind(NODE_KIND::IF_CONDITION);
    
    for(auto node_kind : ContainerNode::getAcceptedNodeKinds()){
        setAcceptsNodeKind(node_kind);
    }
}

bool MEDEA::WhileLoop::canAdoptChild(Node *child)
{
    auto child_kind = child->getNodeKind();
    
    switch(child_kind){
        case NODE_KIND::IF_CONDITION:{
            if(getChildrenOfKind(child_kind, 0).size() > 0){
                return false;
            }
            break;
        }
    default:
        break;
    }
    
    //Ignore the can adopt child from condition
    return Node::canAdoptChild(child);
}

