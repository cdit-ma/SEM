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
    setDataReceiver(true);
    setDataProducer(true);


    addInstancesDefinitionKind(NODE_KIND::ENUM);
    setChainableDefinition();
}

bool EnumInstance::canAcceptEdge(EDGE_KIND edge_kind, Node * dst)
{
    if(canCurrentlyAcceptEdgeKind(edge_kind, dst) == false){
        return false;
    }
    switch(edge_kind){
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
    return DataNode::canAcceptEdge(edge_kind, dst);
}