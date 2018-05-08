#include "enuminstance.h"
#include "../../entityfactory.h"

const NODE_KIND node_kind = NODE_KIND::ENUM_INSTANCE;
const QString kind_string = "EnumInstance";

void EnumInstance::RegisterWithEntityFactory(EntityFactory& factory){
    Node::RegisterWithEntityFactory(factory, node_kind, kind_string, [](EntityFactory& factory, bool is_temp_node){
        return new EnumInstance(factory, is_temp_node);
    });
}

EnumInstance::EnumInstance(EntityFactory& factory, bool is_temp) : DataNode(factory, node_kind, is_temp){
    if(is_temp){
        return;
    }

    //Setup State
    setDataReceiver(true);
    setDataProducer(true);
    addInstancesDefinitionKind(NODE_KIND::ENUM);
    setChainableDefinition();

    //Setup Data
    factory.AttachData(this, "type", QVariant::String, "", true);
    factory.AttachData(this, "value", QVariant::String, "", false);
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