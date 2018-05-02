#include "attributeimpl.h"

#include "../../edgekinds.h"



AttributeImpl::AttributeImpl(EntityFactory* factory) : DataNode(factory, NODE_KIND::ATTRIBUTE_IMPL, "AttributeImpl"){
	auto node_kind = NODE_KIND::ATTRIBUTE_IMPL;
	QString kind_string = "AttributeImpl";
	RegisterNodeKind(factory, node_kind, kind_string, [](){return new AttributeImpl();});

    //Register Data
    RegisterDefaultData(factory, node_kind, "type", QVariant::String, true);
};

AttributeImpl::AttributeImpl():DataNode(NODE_KIND::ATTRIBUTE_IMPL)
{
    addImplsDefinitionKind(NODE_KIND::ATTRIBUTE);
    
    setDataProducer(true);
    setDataReceiver(false);
}

bool AttributeImpl::canAcceptEdge(EDGE_KIND edge_kind, Node *dst)
{
    if(canCurrentlyAcceptEdgeKind(edge_kind, dst) == false){
        return false;
    }

    switch(edge_kind){
    case EDGE_KIND::DEFINITION:{
        if(dst->getNodeKind() != NODE_KIND::ATTRIBUTE){
            return false;
        }
        break;
    }
    case EDGE_KIND::DATA:{
        break;
    }
    default:
        break;
    }
    return DataNode::canAcceptEdge(edge_kind, dst);
}
