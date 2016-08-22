#include "variable.h"

Variable::Variable():DataNode(NK_VARIABLE)
{
    setDataProducer(true);
    setDataReciever(true);
}

bool Variable::canAdoptChild(Node* child)
{
    switch(child->getNodeKind()){
    case NK_AGGREGATE_INSTANCE:
    case NK_VECTOR_INSTANCE:
        break;
    default:
        return false;
    }
    if(hasChildren()){
        return false;
    }

    return DataNode::canAdoptChild(child);
}

bool Variable::canAcceptEdge(Edge::EDGE_CLASS edgeKind, Node *dst)
{
    if(!acceptsEdgeKind(edgeKind)){
        return false;
    }

    switch(edgeKind){
    case Edge::EC_DATA:{
        if(dst->getNodeKind() == NK_VARIABLE || dst->getNodeKind() == NK_ATTRIBUTE_IMPL){
            return false;
        }
        break;
    }
    default:
        break;
    }
    return DataNode::canAcceptEdge(edgeKind, dst);
}
