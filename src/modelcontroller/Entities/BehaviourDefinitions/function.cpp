#include "function.h"

const NODE_KIND node_kind = NODE_KIND::FUNCTION;
const QString kind_string = "Function";

MEDEA::Function::Function(EntityFactory* factory) : Node(factory, node_kind, kind_string){
	RegisterNodeKind(factory, node_kind, kind_string, [](){return new Function();});
};

MEDEA::Function::Function(): Node(node_kind)
{
    
}

bool MEDEA::Function::Function::canAdoptChild(Node* child)
{
    switch(child->getNodeKind()){
        case NODE_KIND::INEVENTPORT_IMPL:
        case NODE_KIND::OUTEVENTPORT_IMPL:
            return false;
        default:
            break;
    }

    if(ContainerNode::canAdoptChild(child)){
        return Node::canAdoptChild(child);
    }
    return false;
}

bool MEDEA::Function::Function::canAcceptEdge(EDGE_KIND, Node *)
{
    return false;
}
