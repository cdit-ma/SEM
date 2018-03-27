#include "requesttype.h"
#include <QDebug>

const NODE_KIND node_kind = NODE_KIND::REQUEST_TYPE;
const QString kind_string = "RequestType";

RequestType::RequestType(EntityFactory* factory) : EventPort(factory, node_kind, kind_string){
    //Allow reordering
    RegisterDefaultData(factory, node_kind, "type", QVariant::String, true);
    RegisterDefaultData(factory, node_kind, "index", QVariant::Int, false);
	RegisterNodeKind(factory, node_kind, kind_string, [](){return new RequestType();});
};

RequestType::RequestType(): EventPort(node_kind)
{
    setDefinitionKind(node_kind);
    setInstanceKind(node_kind);
    setImplKind(node_kind);
    
    setNodeType(NODE_TYPE::INSTANCE);
}


bool RequestType::canAdoptChild(Node* child)
{
    return EventPort::canAdoptChild(child);
}

bool RequestType::canAcceptEdge(EDGE_KIND edge_kind, Node *dst)
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
        if(parent_node && parent_node->getNodeKind() == NODE_KIND::REQUESTREPLY){
            return false;
        }
        
        break;
    }
    default:
        break;
    }

    return EventPort::canAcceptEdge(edge_kind, dst);
}
