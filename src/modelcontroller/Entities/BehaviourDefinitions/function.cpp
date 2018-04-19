#include "function.h"

const NODE_KIND node_kind = NODE_KIND::FUNCTION;
const QString kind_string = "Function";

MEDEA::Function::Function(EntityFactory* factory) : Node(factory, node_kind, kind_string){
	RegisterNodeKind(factory, node_kind, kind_string, [](){return new Function();});
    RegisterDefaultData(factory, node_kind, "icon_prefix", QVariant::String, false);
    RegisterDefaultData(factory, node_kind, "icon", QVariant::String, false);
};

MEDEA::Function::Function(): Node(node_kind)
{
    setNodeType(NODE_TYPE::BEHAVIOUR_CONTAINER);
}

bool MEDEA::Function::Function::canAdoptChild(Node* child)
{
    auto child_kind = child->getNodeKind();
    switch(child_kind){
        case NODE_KIND::INEVENTPORT_IMPL:
        case NODE_KIND::OUTEVENTPORT_IMPL:
            return false;
        default:
            if(!ContainerNode::canAdoptChild(child)){
                return false;
            }
            break;
    }
    return Node::canAdoptChild(child);
}

bool MEDEA::Function::Function::canAcceptEdge(EDGE_KIND, Node *)
{
    return false;
}
