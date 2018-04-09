#include "enuminstance.h"


EnumInstance::EnumInstance(EntityFactory* factory) : DataNode(factory, NODE_KIND::ENUM_INSTANCE, "EnumInstance"){
	auto node_kind = NODE_KIND::ENUM_INSTANCE;
	QString kind_string = "EnumInstance";
    RegisterNodeKind(factory, node_kind, kind_string, [](){return new EnumInstance();});
    
    RegisterDefaultData(factory, node_kind, "type", QVariant::String, true);
    RegisterDefaultData(factory, node_kind, "value", QVariant::String, false);
};

EnumInstance::EnumInstance():DataNode(NODE_KIND::ENUM_INSTANCE)
{
    setNodeType(NODE_TYPE::DEFINITION);
    setNodeType(NODE_TYPE::INSTANCE);
    setAcceptsEdgeKind(EDGE_KIND::DEFINITION);
    setAcceptsEdgeKind(EDGE_KIND::DATA);
    setNodeType(NODE_TYPE::DATA);
    setDataReciever(true);
    setDataProducer(true);


    setDefinitionKind(NODE_KIND::ENUM);
    setInstanceKind(NODE_KIND::ENUM_INSTANCE);
    setImplKind(NODE_KIND::ENUM_INSTANCE);
}

bool EnumInstance::canAdoptChild(Node* child)
{
    return false;
}

bool EnumInstance::canAcceptEdge(EDGE_KIND edgeKind, Node * dst)
{
    if(!acceptsEdgeKind(edgeKind)){
        return false;
    }
    switch(edgeKind){
        case EDGE_KIND::DEFINITION:{
            if(!(dst->getNodeKind() == NODE_KIND::ENUM || dst->getNodeKind() == NODE_KIND::ENUM_INSTANCE)){
                return false;
            }
            break;
        }
    case EDGE_KIND::DATA:{
        bool allow_edge = false;
        if(dst->getNodeKind() == NODE_KIND::ENUM_INSTANCE){
            auto dst_def = dst->getDefinition(true);
            auto enum_def = getDefinition(true);
            if(dst_def == enum_def){
                allow_edge = true;
            }
        }
        if(!allow_edge){
            return false;
        }
    }
    default:
        break;
    }
    return DataNode::canAcceptEdge(edgeKind, dst);
}