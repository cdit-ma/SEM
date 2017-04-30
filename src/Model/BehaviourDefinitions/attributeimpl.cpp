#include "attributeimpl.h"
#include "../nodekinds.h"

AttributeImpl::AttributeImpl():DataNode(NODE_KIND::ATTRIBUTE_IMPL)
{
    setNodeType(NODE_TYPE::IMPLEMENTATION);
    setAcceptsEdgeKind(Edge::EC_DATA);
    setAcceptsEdgeKind(Edge::EC_DEFINITION);

    setDataProducer(true);
    setDataReciever(true);
    setMultipleDataReceiver(true);

    updateDefaultData("type", QVariant::String, true);
}

bool AttributeImpl::canAdoptChild(Node*)
{
    return false;
}

bool AttributeImpl::canAcceptEdge(Edge::EDGE_KIND edgeKind, Node *dst)
{
    if(!acceptsEdgeKind(edgeKind)){
        return false;
    }

    switch(edgeKind){
    case Edge::EC_DEFINITION:{
        if(dst->getNodeKind() != NODE_KIND::ATTRIBUTE){
            return false;
        }
        break;
    }
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
