#include "requestportimpl.h"

const NODE_KIND node_kind = NODE_KIND::REQUEST_PORT_IMPL;
const QString kind_string = "RequestPortImpl";

RequestPortImpl::RequestPortImpl(EntityFactory* factory) : BehaviourNode(factory, node_kind, kind_string){
    //Allow reordering
    RegisterDefaultData(factory, node_kind, "type", QVariant::String, true);
    RegisterDefaultData(factory, node_kind, "index", QVariant::Int, false);
	RegisterNodeKind(factory, node_kind, kind_string, [](){return new RequestPortImpl();});
};

RequestPortImpl::RequestPortImpl(): BehaviourNode(node_kind)
{
    setNodeType(NODE_TYPE::IMPLEMENTATION);
    setAcceptsEdgeKind(EDGE_KIND::DEFINITION);
    setDefinitionKind(NODE_KIND::REQUEST_PORT);
    setWorkflowProducer(true);
    setWorkflowReciever(true);
}


bool RequestPortImpl::canAdoptChild(Node* child)
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
    return BehaviourNode::canAdoptChild(child);
}

bool RequestPortImpl::canAcceptEdge(EDGE_KIND edge_kind, Node * dst)
{
    if(!acceptsEdgeKind(edge_kind)){
        return false;
    }

    switch(edge_kind){
    case EDGE_KIND::DEFINITION:{
        //Can only connect a definition edge to an Aggregate/AggregateInstance..
        if(dst->getNodeKind() != NODE_KIND::REQUEST_PORT){
            return false;
        }
        break;
    }
    default:
        break;
    }
    return BehaviourNode::canAcceptEdge(edge_kind, dst);
}
