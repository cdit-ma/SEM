#include "functioncall.h"
#include <QDebug>
#include "parameter.h"
#include "containernode.h"


const NODE_KIND node_kind = NODE_KIND::FUNCTION_CALL;
const QString kind_string = "FunctionCall";

FunctionCall::FunctionCall():Node(node_kind)
{
    addInstancesDefinitionKind(NODE_KIND::FUNCTION);
}

FunctionCall::FunctionCall(EntityFactory* factory) : Node(factory, node_kind, kind_string){
	RegisterNodeKind(factory, node_kind, kind_string, [](){return new FunctionCall();});

    //Register DefaultData
    RegisterDefaultData(factory, node_kind, "icon", QVariant::String, true);
    RegisterDefaultData(factory, node_kind, "icon_prefix", QVariant::String, true);
    RegisterDefaultData(factory, node_kind, "label", QVariant::String, true);
    RegisterDefaultData(factory, node_kind, "class", QVariant::String, true);

    setAcceptsNodeKind(NODE_KIND::INPUT_PARAMETER_GROUP_INSTANCE);
    setAcceptsNodeKind(NODE_KIND::RETURN_PARAMETER_GROUP_INSTANCE);

    //SetEdgeRuleActive(Node::EdgeRule::ALLOW_EXTERNAL_DEFINITIONS, true);

    addInstancesDefinitionKind(NODE_KIND::FUNCTION);
}


bool FunctionCall::canAdoptChild(Node* child)
{
    auto child_kind = child->getNodeKind();
    switch(child_kind){
        case NODE_KIND::INPUT_PARAMETER_GROUP_INSTANCE:
        case NODE_KIND::RETURN_PARAMETER_GROUP_INSTANCE:{
            if(!getChildrenOfKind(child->getNodeKind(), 0).isEmpty()){
                return false;
            }
            break;
        }
        default:
            break;
    }
    return Node::canAdoptChild(child);
}

QSet<Node*> FunctionCall::getListOfValidAncestorsForChildrenDefinitions(){
    QSet<Node*> valid_ancestors;
    
    //We can create definition edges back to the recursive Definition only, because of the fact the instances are sparse
    auto definition = getDefinition(true);
    if(definition){
        valid_ancestors << definition;
    }

    return valid_ancestors;
}

QSet<Node*> FunctionCall::getParentNodesForValidDefinition(){
    QSet<Node*> parents;
    //Need to look at the CLASS_INSTANCE children contained with in the ComponentImpl/Class
    auto component = getTopBehaviourContainer();
    if(component){

        for(auto node : component->getChildrenOfKind(NODE_KIND::CLASS_INSTANCE, 0)){
            parents << node;
        }
    }
    return parents;
}

Node* FunctionCall::getTopBehaviourContainer(){
    if(!top_behaviour_calculated){
        top_behaviour_container = ContainerNode::getTopBehaviourContainer(this);
        top_behaviour_calculated = true;
    }
    return top_behaviour_container;
}