#include "callbackfunctioninst.h"
#include "../data.h"
#include "../../entityfactorybroker.h"
#include "../../entityfactoryregistrybroker.h"
#include "containernode.h"

const NODE_KIND node_kind = NODE_KIND::CALLBACK_FNC_INST;
const QString kind_string = "Callback Function Instance";

void MEDEA::CallbackFunctionInst::RegisterWithEntityFactory(::EntityFactoryRegistryBroker& broker){
    broker.RegisterWithEntityFactory(node_kind, kind_string, [](::EntityFactoryBroker& broker, bool is_temp_node){
        return new MEDEA::CallbackFunctionInst(broker, is_temp_node);
        });
}

MEDEA::CallbackFunctionInst::CallbackFunctionInst(::EntityFactoryBroker& broker, bool is_temp) : Node(broker, node_kind, is_temp){
    //Setup State
    addInstancesDefinitionKind(NODE_KIND::CALLBACK_FNC);
    SetEdgeRuleActive(Node::EdgeRule::ALWAYS_CHECK_VALID_DEFINITIONS, true);
    SetEdgeRuleActive(Node::EdgeRule::ALWAYS_ALLOW_ADOPTION_AS_INSTANCE, true);
    

    setNodeType(NODE_TYPE::BEHAVIOUR_CONTAINER);

    setAcceptsNodeKind(NODE_KIND::INPUT_PARAMETER_GROUP_INST);
    setAcceptsNodeKind(NODE_KIND::RETURN_PARAMETER_GROUP_INST);

    for(auto node_kind : ContainerNode::getAcceptedNodeKinds()){
        setAcceptsNodeKind(node_kind);
    }

    if(is_temp){
        //Break out early for temporary entities
        return;
    }

    setLabelFunctional(false);

    //Setup Data
    broker.AttachData(this, KeyName::Class, QVariant::String, ProtectedState::PROTECTED);
    broker.AttachData(this, KeyName::Operation, QVariant::String, ProtectedState::UNPROTECTED, "Function");
    broker.AttachData(this, KeyName::IconPrefix, QVariant::String, ProtectedState::UNPROTECTED);
    broker.AttachData(this, KeyName::Icon, QVariant::String, ProtectedState::UNPROTECTED);
}


bool MEDEA::CallbackFunctionInst::CallbackFunctionInst::canAdoptChild(Node* child)
{
    auto child_kind = child->getNodeKind();
    switch(child_kind){
        case NODE_KIND::INPUT_PARAMETER_GROUP_INST:
        case NODE_KIND::RETURN_PARAMETER_GROUP_INST:{
            if(getChildrenOfKindCount(child_kind) > 0){
                return false;
            }
            break;
        }
        default:
            break;
    }
    return Node::canAdoptChild(child);
}

bool MEDEA::CallbackFunctionInst::canAcceptEdge(EDGE_KIND edge_kind, Node *dst)
{
    switch(edge_kind){
    case EDGE_KIND::DEFINITION:{
        if(dst->getInstances().count() > 0){
            return false;
        }
        break;
    }
    default:
        break;
    }
    return Node::canAcceptEdge(edge_kind, dst);
}


QSet<Node*> MEDEA::CallbackFunctionInst::getListOfValidAncestorsForChildrenDefinitions(){
    QSet<Node*> valid_ancestors;
    
    //We can create definition edges back to the recursive Definition only, because of the fact the instances are sparse
    auto definition = getDefinition(true);
    if(definition){
        valid_ancestors << definition;
    }

    return valid_ancestors;
}

QSet<Node*> MEDEA::CallbackFunctionInst::getParentNodesForValidDefinition(){
    QSet<Node*> parents;
    //Need to look at the CLASS_INST children contained with in the ComponentImpl/Class
    auto component = getTopBehaviourContainer();
    if(component){
        parents << component;
        for(auto node : component->getChildrenOfKind(NODE_KIND::CLASS_INST, 0)){
            parents << node;
        }
    }
    return parents;
}

Node* MEDEA::CallbackFunctionInst::getTopBehaviourContainer(){
    if(!top_behaviour_calculated){
        top_behaviour_container = ContainerNode::getTopBehaviourContainer(this);
        top_behaviour_calculated = true;
    }
    return top_behaviour_container;
}
