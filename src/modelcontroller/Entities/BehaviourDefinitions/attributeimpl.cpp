#include "attributeimpl.h"

#include "../../edgekinds.h"

const NODE_KIND node_kind = NODE_KIND::ATTRIBUTE_IMPL;
const QString kind_string = "AttributeImpl";


void AttributeImpl::RegisterWithEntityFactory(EntityFactory& factory){
    Node::RegisterWithEntityFactory(factory, node_kind, kind_string, [](EntityFactory& factory, bool is_temp_node){
        return new AttributeImpl(factory, is_temp_node);
        });
}

AttributeImpl::AttributeImpl(EntityFactory& factory, bool is_temp) : DataNode(factory, node_kind, is_temp){
    if(is_temp){
        return;
    }

    //Setup State
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
