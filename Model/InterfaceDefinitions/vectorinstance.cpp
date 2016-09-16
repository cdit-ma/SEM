#include "vectorinstance.h"

VectorInstance::VectorInstance(): DataNode(Node::NK_VECTOR_INSTANCE)
{
    //Can be both an input/output for data.
    setDataProducer(true);
    setDataReciever(true);
    setAcceptsEdgeKind(Edge::EC_DEFINITION);
    setNodeType(NT_INSTANCE);
}

bool VectorInstance::canAdoptChild(Node *child)
{
    //Can only adopt a MemberInstance/AggregateInstance
    switch(child->getNodeKind()){
    case NK_MEMBER_INSTANCE:
    case NK_AGGREGATE_INSTANCE:{
        if(hasChildren()){
            return false;
        }
        break;
    }
    default:
        return false;
    }

    return Node::canAdoptChild(child);
}

bool VectorInstance::canAcceptEdge(Edge::EDGE_KIND edgeKind, Node *dst)
{
    if(!acceptsEdgeKind(edgeKind)){
        return false;
    }
    switch(edgeKind){
    case Edge::EC_DEFINITION:{
        if(!(dst->getNodeKind() == NK_VECTOR_INSTANCE || dst->getNodeKind() == NK_VECTOR)){
            return false;
        }
        break;
    }
    case Edge::EC_DATA:{
        if(dst->getNodeKind() != NK_VECTOR_INSTANCE){
            return false;
        }
        if(dst->getDefinition(true) != getDefinition(true)){
            return false;
        }
        break;
    }
    default:
        break;
    }
    return DataNode::canAcceptEdge(edgeKind, dst);
}
