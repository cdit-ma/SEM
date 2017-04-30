#include "vectorinstance.h"

VectorInstance::VectorInstance(): DataNode(NODE_KIND::VECTOR_INSTANCE)
{
    //Can be both an input/output for data.
    setDataProducer(true);
    setDataReciever(true);
    setAcceptsEdgeKind(Edge::EC_DEFINITION);
    setNodeType(NODE_TYPE::INSTANCE);
    setNodeType(NODE_TYPE::DEFINITION);

    updateDefaultData("label", QVariant::String, true, getNodeKindStr());
    updateDefaultData("type", QVariant::String, true);
}

bool VectorInstance::canAdoptChild(Node *child)
{
    //Can only adopt a MemberInstance/AggregateInstance
    switch(child->getNodeKind()){
    case NODE_KIND::MEMBER_INSTANCE:
    case NODE_KIND::AGGREGATE_INSTANCE:{
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
        if(!(dst->getNodeKind() == NODE_KIND::VECTOR_INSTANCE || dst->getNodeKind() == NODE_KIND::VECTOR)){
            return false;
        }
        break;
    }
    case Edge::EC_DATA:{
        if(dst->getNodeKind() == NODE_KIND::VECTOR_INSTANCE){
            if(getDefinition(true) && dst->getDefinition(true) != getDefinition(true)){
                return false;
            }
        }

        break;
    }
    default:
        break;
    }
    return DataNode::canAcceptEdge(edgeKind, dst);
}
