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

        setAcceptsEdgeKind(EDGE_KIND::DEFINITION);
    setNodeType(NODE_TYPE::INSTANCE);
    setNodeType(NODE_TYPE::DEFINITION);

    setInstanceKind(NODE_KIND::FUNCTION);
    setImplKind(NODE_KIND::FUNCTION);
    setDefinitionKind(NODE_KIND::FUNCTION);
}

bool MEDEA::Function::Function::canAdoptChild(Node* child)
{
    auto child_kind = child->getNodeKind();
    switch(child_kind){
        case NODE_KIND::INEVENTPORT_IMPL:
        case NODE_KIND::OUTEVENTPORT_IMPL:
        case NODE_KIND::SERVER_REQUEST:
            return false;
        // Should be replaced by parameter groups when they're ready
        case NODE_KIND::INPUT_PARAMETER:
        case NODE_KIND::RETURN_PARAMETER:
        case NODE_KIND::VARIABLE_PARAMETER:
            break;
        default:
            if(!ContainerNode::canAdoptChild(child)){
                return false;
            }
            break;
    }
    return Node::canAdoptChild(child);
}

bool MEDEA::Function::Function::canAcceptEdge(EDGE_KIND edgeKind, Node *dst)
{
    if(!acceptsEdgeKind(edgeKind)){
        return false;
    }

    switch(edgeKind){
        case EDGE_KIND::DEFINITION:{
            if(dst->getNodeKind() != NODE_KIND::FUNCTION){
                return false;
            }

            
            if(getParentNode()->getNodeKind() == NODE_KIND::COMPONENT_IMPL){

                if(!dst->getDefinition()){
                    return false;
                }
            }
            
            //return true;
            break;
        }
        default:
            break;
    }

    return Node::canAcceptEdge(edgeKind, dst);
}
