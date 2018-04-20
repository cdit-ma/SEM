#include "returnparametergroup.h"
#include <QDebug>

const NODE_KIND node_kind = NODE_KIND::RETURN_PARAMETER_GROUP;
const QString kind_string = "ReturnParameterGroup";

MEDEA::ReturnParameterGroup::ReturnParameterGroup(EntityFactory* factory) : Node(factory, node_kind, kind_string){
    //Allow reordering
    RegisterDefaultData(factory, node_kind, "type", QVariant::String, true);
    RegisterDefaultData(factory, node_kind, "index", QVariant::Int, false);
	RegisterNodeKind(factory, node_kind, kind_string, [](){return new ReturnParameterGroup();});
};

MEDEA::ReturnParameterGroup::ReturnParameterGroup(): Node(node_kind)
{
    setAcceptsEdgeKind(EDGE_KIND::DEFINITION);
    setDefinitionKind(node_kind);
    setInstanceKind(node_kind);
    setImplKind(node_kind);
    
    setNodeType(NODE_TYPE::INSTANCE);
    setNodeType(NODE_TYPE::DEFINITION);
}


bool MEDEA::ReturnParameterGroup::canAdoptChild(Node* child)
{
    switch(child->getNodeKind()){
        case NODE_KIND::INPUT_PARAMETER:
        case NODE_KIND::AGGREGATE_INSTANCE:
            break;
    default:
        return false;
    }

    if(childrenCount() > 0 ){
        return false;
    }
    return Node::canAdoptChild(child);
}

bool MEDEA::ReturnParameterGroup::canAcceptEdge(EDGE_KIND edge_kind, Node *dst)
{
    if(!acceptsEdgeKind(edge_kind)){
        return false;
    }

    switch(edge_kind){
    case EDGE_KIND::DEFINITION:{
        //Can only connect a definition edge to an Aggregate/AggregateInstance..
        if(dst->getNodeKind() != node_kind){
            return false;
        }
        auto parent_node = getParentNode();
        if(parent_node && parent_node->getNodeKind() == NODE_KIND::SERVER_INTERFACE){
            return false;
        }
        
        break;
    }
    default:
        break;
    }

    return Node::canAcceptEdge(edge_kind, dst);
}
