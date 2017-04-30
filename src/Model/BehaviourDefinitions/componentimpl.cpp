#include "componentimpl.h"
#include "../nodekinds.h"

ComponentImpl::ComponentImpl():Node(NODE_KIND::COMPONENT_IMPL){
    setNodeType(NODE_TYPE::IMPLEMENTATION);
    setAcceptsEdgeKind(Edge::EC_DEFINITION);

    updateDefaultData("type", QVariant::String, true);
}

bool ComponentImpl::canAdoptChild(Node *child)
{
    switch(child->getNodeKind()){
        case NODE_KIND::ATTRIBUTE_IMPL:
        case NODE_KIND::INEVENTPORT_IMPL:
        case NODE_KIND::OUTEVENTPORT_IMPL:
        case NODE_KIND::BRANCH_STATE:
        case NODE_KIND::PERIODICEVENT:
        case NODE_KIND::TERMINATION:
        case NODE_KIND::VARIABLE:
        case NODE_KIND::WORKLOAD:
        case NODE_KIND::PROCESS:
        case NODE_KIND::WORKER_PROCESS:
        case NODE_KIND::WHILELOOP:
        case NODE_KIND::SETTER:
        case NODE_KIND::CODE:
        case NODE_KIND::HEADER:
            break;
    default:
        return false;
    }
    return Node::canAdoptChild(child);
}

bool ComponentImpl::canAcceptEdge(Edge::EDGE_KIND edgeKind, Node *dst)
{
    if(!acceptsEdgeKind(edgeKind)){
        return false;
    }

    switch(edgeKind){
    case Edge::EC_DEFINITION:{
        if(!dst->getImplementations().isEmpty()){
            return false;
        }
        if(dst->getNodeKind() != NODE_KIND::COMPONENT){
            return false;
        }
        break;
    }
    default:
        break;
    }
    return Node::canAcceptEdge(edgeKind, dst);
}
