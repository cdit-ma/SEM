#include "componentimpl.h"

ComponentImpl::ComponentImpl():Node(Node::NK_COMPONENT_IMPL){
    setNodeType(NT_IMPLEMENTATION);
    setAcceptsEdgeKind(Edge::EC_DEFINITION);

    updateDefaultData("type", QVariant::String, true);
}

bool ComponentImpl::canAdoptChild(Node *child)
{
    switch(child->getNodeKind()){
        case Node::NK_ATTRIBUTE_IMPL:
        case Node::NK_INEVENTPORT_IMPL:
        case Node::NK_OUTEVENTPORT_IMPL:
        case Node::NK_BRANCH_STATE:
        case Node::NK_PERIODICEVENT:
        case Node::NK_TERMINATION:
        case Node::NK_VARIABLE:
        case Node::NK_WORKLOAD:
        case Node::NK_PROCESS:
        case Node::NK_WORKER_PROCESS:
        case Node::NK_WHILELOOP:
        case Node::NK_SETTER:
        case Node::NK_CODE:
        case Node::NK_HEADER:
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
        if(dst->getNodeKind() != NK_COMPONENT){
            return false;
        }
        break;
    }
    default:
        break;
    }
    return Node::canAcceptEdge(edgeKind, dst);
}
