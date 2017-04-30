#include "memberinstance.h"

MemberInstance::MemberInstance():DataNode(NODE_KIND::MEMBER_INSTANCE)
{
    setNodeType(NODE_TYPE::DEFINITION);
    setNodeType(NODE_TYPE::INSTANCE);
    setAcceptsEdgeKind(Edge::EC_DEFINITION);
    setDataReciever(true);
    setDataProducer(true);

    
    updateDefaultData("type", QVariant::String, true);
}

bool MemberInstance::canAdoptChild(Node*)
{
    return false;
}

bool MemberInstance::canAcceptEdge(Edge::EDGE_KIND edgeKind, Node *dst)
{
    if(!acceptsEdgeKind(edgeKind)){
        return false;
    }
    switch(edgeKind){
        case Edge::EC_DEFINITION:{
            if(!(dst->getNodeKind() == NODE_KIND::MEMBER || dst->getNodeKind() == NODE_KIND::MEMBER_INSTANCE)){
                return false;
            }
            break;
        }
    case Edge::EC_WORKFLOW:{
        return false;
    }
    default:
        break;
    }
    return DataNode::canAcceptEdge(edgeKind, dst);
}
