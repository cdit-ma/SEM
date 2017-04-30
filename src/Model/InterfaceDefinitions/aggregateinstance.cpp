#include "aggregateinstance.h"

AggregateInstance::AggregateInstance():DataNode(NODE_KIND::AGGREGATE_INSTANCE)
{
    setAcceptsEdgeKind(Edge::EC_DEFINITION);
    setNodeType(NODE_TYPE::INSTANCE);
    setNodeType(NODE_TYPE::DEFINITION);

    setDataProducer(true);
    setDataReciever(true);

    updateDefaultData("label", QVariant::String, false, getNodeKindStr());
    updateDefaultData("type", QVariant::String, false);
}

bool AggregateInstance::canAdoptChild(Node *child)
{
    switch(child->getNodeKind()){
    case NODE_KIND::AGGREGATE_INSTANCE:
    case NODE_KIND::MEMBER_INSTANCE:
    case NODE_KIND::VECTOR_INSTANCE:
        break;
    default:
        return false;
    }
    return DataNode::canAdoptChild(child);
}

bool AggregateInstance::canAcceptEdge(Edge::EDGE_KIND edgeKind, Node *dst)
{
    if(!acceptsEdgeKind(edgeKind)){
        return false;
    }
    switch(edgeKind){
    case Edge::EC_DATA:{
        //Can only connect to an AggregateInstance.
        if(dst->getNodeKind() == NODE_KIND::AGGREGATE_INSTANCE){
            //Can only connect to an AggregateInstance with the same definition.
            if(!getDefinition(true) || getDefinition(true) != dst->getDefinition(true)){
                return false;
            }
        }

        break;
    }
    case Edge::EC_DEFINITION:{

        //Can only connect a definition edge to an Aggregate/AggregateInstance..
        if(!(dst->getNodeKind() == NODE_KIND::AGGREGATE_INSTANCE || dst->getNodeKind() == NODE_KIND::AGGREGATE)){
            return false;
        }
        break;
    }
    default:
        break;

    }
    return DataNode::canAcceptEdge(edgeKind, dst);
}

