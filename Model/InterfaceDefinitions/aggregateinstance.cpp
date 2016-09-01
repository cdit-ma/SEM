#include "aggregateinstance.h"

AggregateInstance::AggregateInstance():DataNode(Node::NK_AGGREGATE_INSTANCE)
{
    setAcceptsEdgeKind(Edge::EC_DEFINITION);
    setNodeType(NT_INSTANCE);
    setNodeType(NT_DEFINITION);

    setDataProducer(true);
    setDataReciever(true);
}

bool AggregateInstance::canAdoptChild(Node *child)
{
    switch(child->getNodeKind()){
    case NK_AGGREGATE_INSTANCE:
    case NK_MEMBER_INSTANCE:
    case NK_VECTOR_INSTANCE:
        break;
    default:
        return false;
    }
    return DataNode::canAdoptChild(child);
}

bool AggregateInstance::canAcceptEdge(Edge::EDGE_CLASS edgeKind, Node *dst)
{
    if(!acceptsEdgeKind(edgeKind)){
        return false;
    }
    switch(edgeKind){
    case Edge::EC_DATA:{
        //Can only connect to an AggregateInstance.
        if(dst->getNodeKind() != NK_AGGREGATE_INSTANCE){
            return false;
        }

        //Can only connect to an AggregateInstance with the same definition.
        if(getDefinition(true) != dst->getDefinition(true)){
            return false;
        }
        break;
    }
    case Edge::EC_DEFINITION:{

        //Can only connect a definition edge to an Aggregate/AggregateInstance..
        if(!(dst->getNodeKind() == NK_AGGREGATE_INSTANCE || dst->getNodeKind() == NK_AGGREGATE)){
            return false;
        }
        break;
    }
    default:
        break;

    }
    return DataNode::canAcceptEdge(edgeKind, dst);
}
