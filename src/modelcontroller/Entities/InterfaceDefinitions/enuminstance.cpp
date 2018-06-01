#include "enuminstance.h"
#include "aggregateinstance.h"
#include "../../entityfactorybroker.h"
#include "../../entityfactoryregistrybroker.h"
#include "../../entityfactoryregistrybroker.h"

const NODE_KIND node_kind = NODE_KIND::ENUM_INSTANCE;
const QString kind_string = "Enum Instance";

void EnumInstance::RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker){
    broker.RegisterWithEntityFactory(node_kind, kind_string, [](EntityFactoryBroker& broker, bool is_temp_node){
        return new EnumInstance(broker, is_temp_node);
    });
}

EnumInstance::EnumInstance(EntityFactoryBroker& broker, bool is_temp) : DataNode(broker, node_kind, is_temp){
    //Setup State
    addInstancesDefinitionKind(NODE_KIND::ENUM);
    setChainableDefinition();

    if(is_temp){
        //Break out early for temporary entities
        return;
    }

    //Setup Data
    broker.AttachData(this, "type", QVariant::String, ProtectedState::PROTECTED);
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

void EnumInstance::parentSet(Node* parent){
    switch(parent->getNodeKind()){
        case NODE_KIND::AGGREGATE:{
            getFactoryBroker().AttachData(this, "index", QVariant::Int, ProtectedState::UNPROTECTED);
            SetEdgeRuleActive(EdgeRule::DISALLOW_DEFINITION_CHAINING);
            break;
        }
        case NODE_KIND::VECTOR:
        case NODE_KIND::VARIABLE:
        case NODE_KIND::INPUT_PARAMETER_GROUP:
        case NODE_KIND::RETURN_PARAMETER_GROUP:{
            SetEdgeRuleActive(EdgeRule::DISALLOW_DEFINITION_CHAINING);
            break;
        }
        default:
            break;
    }

    AggregateInstance::ParentSet(this);
    DataNode::parentSet(parent);
}
