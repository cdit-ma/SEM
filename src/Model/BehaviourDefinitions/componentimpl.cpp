#include "componentimpl.h"
#include "../nodekinds.h"

//#include "../entityfactory.h"

ComponentImpl::ComponentImpl(EntityFactory* factory) : Node(factory, NODE_KIND::COMPONENT_IMPL, "ComponentImpl"){
	auto node_kind = NODE_KIND::COMPONENT_IMPL;
	QString kind_string = "ComponentImpl";
	RegisterNodeKind(factory, node_kind, kind_string, [](){return new ComponentImpl();});

    //Register DefaultData
    RegisterDefaultData(factory, node_kind, "type", QVariant::String, true);
};

ComponentImpl::ComponentImpl():Node(NODE_KIND::COMPONENT_IMPL){
    setNodeType(NODE_TYPE::IMPLEMENTATION);
    setAcceptsEdgeKind(EDGE_KIND::DEFINITION);

    setDefinitionKind(NODE_KIND::COMPONENT);
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

bool ComponentImpl::canAcceptEdge(EDGE_KIND edgeKind, Node *dst)
{
    if(!acceptsEdgeKind(edgeKind)){
        return false;
    }

    switch(edgeKind){
    case EDGE_KIND::DEFINITION:{
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
