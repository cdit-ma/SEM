#include "classinst.h"
#include "../../entityfactorybroker.h"
#include "../../entityfactoryregistrybroker.h"

const NODE_KIND node_kind = NODE_KIND::CLASS_INST;
const QString kind_string = "Class Instance";

void MEDEA::ClassInst::RegisterWithEntityFactory(::EntityFactoryRegistryBroker& broker){
    broker.RegisterWithEntityFactory(node_kind, kind_string, [](::EntityFactoryBroker& broker, bool is_temp_node){
        return new MEDEA::ClassInst(broker, is_temp_node);
        });
}

MEDEA::ClassInst::ClassInst(::EntityFactoryBroker& broker, bool is_temp) : Node(broker, node_kind, is_temp){
    //Setup State
    addInstancesDefinitionKind(NODE_KIND::CLASS);
    setChainableDefinition();
    
    SetEdgeRuleActive(Node::EdgeRule::ALLOW_EXTERNAL_DEFINITIONS, true);

    setAcceptsNodeKind(NODE_KIND::ATTRIBUTE_INST);

    if(is_temp){
        //Break out early for temporary entities
        return;
    }

    //Setup Data
    broker.AttachData(this, KeyName::Type, QVariant::String, ProtectedState::PROTECTED);
    broker.AttachData(this, KeyName::IconPrefix, QVariant::String, ProtectedState::PROTECTED);
    broker.AttachData(this, KeyName::Icon, QVariant::String, ProtectedState::PROTECTED);
    broker.AttachData(this, KeyName::Index, QVariant::Int, ProtectedState::UNPROTECTED);
    broker.AttachData(this, KeyName::Row, QVariant::Int, ProtectedState::PROTECTED, 1);
    broker.AttachData(this, KeyName::Column, QVariant::Int, ProtectedState::PROTECTED, 2);
    broker.AttachData(this, KeyName::IsWorker, QVariant::Bool, ProtectedState::PROTECTED);
};

bool MEDEA::ClassInst::ClassInst::canAcceptEdge(EDGE_KIND edge_kind, Node* dst)
{
    if(canCurrentlyAcceptEdgeKind(edge_kind, dst) == false){
        return false;
    }

    auto parent_node = getParentNode();
    auto parent_node_kind = parent_node ? parent_node->getNodeKind() : NODE_KIND::NONE;

    switch(edge_kind){
    case EDGE_KIND::DEFINITION:{
        switch(dst->getNodeKind()){
            case NODE_KIND::CLASS:
            case NODE_KIND::CLASS_INST:{
                if(parent_node_kind == NODE_KIND::COMPONENT_INST){
                    auto parent_node_def = parent_node->getDefinition(true);
                    bool in_ancestor = false;
                    if(parent_node_def){
                        for(auto impl : parent_node_def->getImplementations()){
                            if(impl->isAncestorOf(dst)){
                                in_ancestor = true;
                                break;
                            }
                        }
                    }
                    
                    if(!in_ancestor){
                        return false;
                    }
                }else{
                    if(dst->getNodeKind() != NODE_KIND::CLASS){
                        return false;
                    }
                }
                break;
            }
            default:
                return false;
        }
        break;
    }
    default:
        break;
    }

    return Node::canAcceptEdge(edge_kind, dst);
}

void MEDEA::ClassInst::parentSet(Node*){
    bool allow_children = false;

    switch(getViewAspect()){
        case VIEW_ASPECT::BEHAVIOUR:
        case VIEW_ASPECT::WORKERS:{
            allow_children = true;
            break;
        }
        default:{
            break;
        }
    }

    if(allow_children){
        //Behaviour
        setAcceptsNodeKind(NODE_KIND::FUNCTION);
        setAcceptsNodeKind(NODE_KIND::CALLBACK_FNC);
        setAcceptsNodeKind(NODE_KIND::CLASS_INST);
    }
}