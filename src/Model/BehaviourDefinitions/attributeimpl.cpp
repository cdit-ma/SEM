#include "attributeimpl.h"
#include "../nodekinds.h"

#include "../entityfactory.h"

AttributeImpl::AttributeImpl(EntityFactory* factory) : DataNode(factory, NODE_KIND::ATTRIBUTE_IMPL, "AttributeImpl"){
	auto node_kind = NODE_KIND::ATTRIBUTE_IMPL;
	QString kind_string = "AttributeImpl";
	RegisterNodeKind(factory, node_kind, kind_string, [](){return new AttributeImpl();});

    //Register Data
    RegisterDefaultData(factory, node_kind, "type", QVariant::String, true);
};

AttributeImpl::AttributeImpl():DataNode(NODE_KIND::ATTRIBUTE_IMPL)
{
    setNodeType(NODE_TYPE::IMPLEMENTATION);
    setAcceptsEdgeKind(EDGE_KIND::DATA);
    setAcceptsEdgeKind(EDGE_KIND::DEFINITION);

    setDefinitionKind(NODE_KIND::ATTRIBUTE);

    setDataProducer(true);
    setDataReciever(true);
    setMultipleDataReceiver(true);
}

bool AttributeImpl::canAdoptChild(Node*)
{
    return false;
}

bool AttributeImpl::canAcceptEdge(EDGE_KIND edgeKind, Node *dst)
{
    if(!acceptsEdgeKind(edgeKind)){
        return false;
    }

    switch(edgeKind){
    case EDGE_KIND::DEFINITION:{
        if(dst->getNodeKind() != NODE_KIND::ATTRIBUTE){
            return false;
        }
        break;
    }
    case EDGE_KIND::DATA:{
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
