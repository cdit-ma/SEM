#include "function.h"
#include "behaviournode.h"

const NODE_KIND node_kind = NODE_KIND::FUNCTION;
const QString kind_string = "Function";

MEDEA::Function::Function(EntityFactory* factory) : ContainerNode(factory, node_kind, kind_string){
	RegisterNodeKind(factory, node_kind, kind_string, [](){return new Function();});
};

MEDEA::Function::Function(): ContainerNode(node_kind)
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

    return ContainerNode::canAdoptChild(child);
}

bool MEDEA::Function::Function::canAcceptEdge(EDGE_KIND, Node *)
{
    return false;
}
