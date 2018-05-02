#include "function.h"
#include "../data.h"
const NODE_KIND node_kind = NODE_KIND::FUNCTION;
const QString kind_string = "Function";

MEDEA::Function::Function(EntityFactory* factory) : Node(factory, node_kind, kind_string){
	RegisterNodeKind(factory, node_kind, kind_string, [](){return new Function();});
    RegisterDefaultData(factory, node_kind, "icon_prefix", QVariant::String, false);
    RegisterDefaultData(factory, node_kind, "icon", QVariant::String, false);
    RegisterDefaultData(factory, node_kind, "class", QVariant::String, true);
};

MEDEA::Function::Function(): Node(node_kind)
{
    setNodeType(NODE_TYPE::BEHAVIOUR_CONTAINER);
    setChainableDefinition();
    
    addInstanceKind(NODE_KIND::FUNCTION_CALL);
    
    setAcceptsNodeKind(NODE_KIND::INPUT_PARAMETER_GROUP);
    setAcceptsNodeKind(NODE_KIND::RETURN_PARAMETER_GROUP);

    for(auto node_kind : ContainerNode::getAcceptedNodeKinds()){
        setAcceptsNodeKind(node_kind);
    }
}

bool MEDEA::Function::Function::canAdoptChild(Node* child)
{
    auto child_kind = child->getNodeKind();
    switch(child_kind){
        case NODE_KIND::INPUT_PARAMETER_GROUP:
        case NODE_KIND::RETURN_PARAMETER_GROUP:{
            if(!getChildrenOfKind(child->getNodeKind(), 0).isEmpty()){
                return false;
            }
            break;
        }
        default:
            break;
    }
    return Node::canAdoptChild(child);
}

bool MEDEA::Function::Function::canAcceptEdge(EDGE_KIND edge_kind, Node *dst)
{   /*
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
            break;
        }
        default:
            break;
    }*/

    return Node::canAcceptEdge(edge_kind, dst);
}

void MEDEA::Function::parentSet(Node* parent){

    auto src_data = parent->getData("type");
    auto dst_data = getData("class");
    if(src_data && dst_data){
        src_data->linkData(dst_data, true);
    }

    

    auto parent_node_kind = parent->getNodeKind();
    if(parent_node_kind != NODE_KIND::COMPONENT_IMPL){
        setAcceptsNodeKind(NODE_KIND::INEVENTPORT_IMPL, false);
        setAcceptsNodeKind(NODE_KIND::OUTEVENTPORT_IMPL, false);
        setAcceptsNodeKind(NODE_KIND::SERVER_REQUEST, false);
    }

    Node::parentSet(parent);
}
