#include "variable.h"

Variable::Variable():DataNode(NODE_KIND::VARIABLE)
{
    setDataProducer(true);
    setDataReciever(true);
    setMultipleDataReceiver(true);

    updateDefaultData("type", QVariant::String, false, "String");
    updateDefaultData("value", QVariant::String);
}

bool Variable::canAdoptChild(Node* child)
{
    switch(child->getNodeKind()){
    case NODE_KIND::AGGREGATE_INSTANCE:
    case NODE_KIND::VECTOR:
        break;
    default:
        return false;
    }
    if(hasChildren()){
        return false;
    }

    return DataNode::canAdoptChild(child);
}

bool Variable::canAcceptEdge(Edge::EDGE_KIND edgeKind, Node *dst)
{
    if(!acceptsEdgeKind(edgeKind)){
        return false;
    }

    switch(edgeKind){
    case Edge::EC_DATA:{
        if(dst->getNodeKind() == NODE_KIND::VARIABLE || dst->getNodeKind() == NODE_KIND::ATTRIBUTE_IMPL){
            return false;
        }
        break;
    }
    default:
        break;
    }
    return DataNode::canAcceptEdge(edgeKind, dst);
}
