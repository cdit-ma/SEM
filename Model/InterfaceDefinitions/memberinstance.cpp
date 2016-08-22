#include "memberinstance.h"

MemberInstance::MemberInstance():DataNode(NK_MEMBER_INSTANCE)
{
    setNodeType(NT_DEFINITION);
    setNodeType(NT_INSTANCE);
}

bool MemberInstance::canAdoptChild(Node*)
{
    return false;
}

bool MemberInstance::canAcceptEdge(Edge::EDGE_CLASS edgeKind, Node *dst)
{
    if(!acceptsEdgeKind(edgeKind)){
        return false;
    }
    switch(edgeKind){
        case Edge::EC_DEFINITION:{
            if(!(dst->getNodeKind() == NK_MEMBER || dst->getNodeKind() == NK_MEMBER_INSTANCE)){
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
