#include "requestreply.h"

const NODE_KIND node_kind = NODE_KIND::REQUESTREPLY;
const QString kind_string = "RequestReply";

RequestReply::RequestReply(EntityFactory* factory) : Node(factory, node_kind, kind_string){
    //Allow reordering
    RegisterDefaultData(factory, node_kind, "index", QVariant::Int, false);
	RegisterNodeKind(factory, node_kind, kind_string, [](){return new RequestReply();});
};

RequestReply::RequestReply(): Node(node_kind)
{
    setNodeType(NODE_TYPE::DEFINITION);
    setAcceptsEdgeKind(EDGE_KIND::DEFINITION);
}


bool RequestReply::canAdoptChild(Node* child)
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

bool RequestReply::canAcceptEdge(EDGE_KIND edge_kind, Node * dst)
{
    return Node::canAcceptEdge(edge_kind, dst);
}
