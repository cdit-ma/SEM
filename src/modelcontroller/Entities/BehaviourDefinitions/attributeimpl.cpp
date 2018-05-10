#include "attributeimpl.h"
#include "../../entityfactorybroker.h"
#include "../../entityfactoryregistrybroker.h"

const NODE_KIND node_kind = NODE_KIND::ATTRIBUTE_IMPL;
const QString kind_string = "AttributeImpl";


void AttributeImpl::RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker){
    broker.RegisterWithEntityFactory(node_kind, kind_string, [](EntityFactoryBroker& broker, bool is_temp_node){
        return new AttributeImpl(broker, is_temp_node);
        });
}

AttributeImpl::AttributeImpl(EntityFactoryBroker& broker, bool is_temp) : DataNode(broker, node_kind, is_temp){
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
