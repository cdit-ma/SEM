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
    setChainableDefinition();
    
    addInstanceKind(NODE_KIND::FUNCTION_CALL);
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
        case NODE_KIND::INPUT_PARAMETER_GROUP:
        case NODE_KIND::RETURN_PARAMETER_GROUP:{
            if(!getChildrenOfKind(child->getNodeKind(), 0).isEmpty()){
                return false;
            }
            break;
        }
        default:
            if(!ContainerNode::canAdoptChild(child)){
                return false;
            }
            break;
    }
    return Node::canAdoptChild(child);
}

bool MEDEA::Function::Function::canAcceptEdge(EDGE_KIND edge_kind, Node *dst)
{
    if(canCurrentlyAcceptEdgeKind(edge_kind, dst) == false){
        return false;
    }

    switch(edge_kind){
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

    return Node::canAcceptEdge(edge_kind, dst);
}
