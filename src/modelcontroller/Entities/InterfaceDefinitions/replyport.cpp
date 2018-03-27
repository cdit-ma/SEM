#include "replyport.h"

const NODE_KIND node_kind = NODE_KIND::REPLY_PORT;
const QString kind_string = "ReplyPort";

ReplyPort::ReplyPort(EntityFactory* factory) : Node(factory, node_kind, kind_string){
    //Allow reordering
    RegisterDefaultData(factory, node_kind, "type", QVariant::String, true);
    RegisterDefaultData(factory, node_kind, "index", QVariant::Int, false);
	RegisterNodeKind(factory, node_kind, kind_string, [](){return new ReplyPort();});
};

ReplyPort::ReplyPort(): Node(node_kind)
{
    setAcceptsEdgeKind(EDGE_KIND::DEFINITION);
    setNodeType(NODE_TYPE::INSTANCE);
    setNodeType(NODE_TYPE::DEFINITION);

    setDefinitionKind(NODE_KIND::REQUESTREPLY);
    setImplKind(NODE_KIND::REPLY_PORT_IMPL);
    setInstanceKind(NODE_KIND::REPLY_PORT_INSTANCE);
}


bool ReplyPort::canAdoptChild(Node* child)
{
    switch(child->getNodeKind()){
    case NODE_KIND::REQUEST_TYPE:
    case NODE_KIND::REPLY_TYPE:{
        if(!getChildrenOfKind(child->getNodeKind(), 0).isEmpty()){
            return false;
        }
        break;
    }
    default:
        return false;
    };
    return Node::canAdoptChild(child);
}

bool ReplyPort::canAcceptEdge(EDGE_KIND edge_kind, Node * dst)
{
    if(!acceptsEdgeKind(edge_kind)){
        return false;
    }
    switch(edge_kind){
    case EDGE_KIND::DEFINITION:{
        //Can only connect a definition edge to an Aggregate/AggregateInstance..
        if(!(dst->getNodeKind() == NODE_KIND::REQUESTREPLY || dst->getNodeKind() == NODE_KIND::REPLY_PORT)){
            return false;
        }
        break;
    }
    default:
        break;
    }
    return Node::canAcceptEdge(edge_kind, dst);
}

