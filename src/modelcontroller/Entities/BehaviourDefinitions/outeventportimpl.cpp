#include "outeventportimpl.h"

#include "../../edgekinds.h"
#include "containernode.h"

const NODE_KIND node_kind = NODE_KIND::OUTEVENTPORT_IMPL;
const QString kind_string = "OutEventPortImpl";


OutEventPortImpl::OutEventPortImpl(EntityFactory* factory) : Node(factory, node_kind, kind_string){
	RegisterNodeKind(factory, node_kind, kind_string, [](){return new OutEventPortImpl();});
    RegisterDefaultData(factory, node_kind, "type", QVariant::String, true);
};

OutEventPortImpl::OutEventPortImpl():Node(node_kind){
    addImplsDefinitionKind(NODE_KIND::OUTEVENTPORT);

    setAcceptsNodeKind(NODE_KIND::AGGREGATE_INSTANCE);

    setNodeType(NODE_TYPE::BEHAVIOUR_ELEMENT);
    SetEdgeRuleActive(Node::EdgeRule::ALWAYS_CHECK_VALID_DEFINITIONS, true);
}

bool OutEventPortImpl::canAdoptChild(Node *child)
{
    auto child_node_kind = child->getNodeKind();
    
    switch(child_node_kind){
        case NODE_KIND::AGGREGATE_INSTANCE:{
            if(getChildrenOfKind(child_node_kind, 0).size() > 0){
                return false;
            }
            break;
        default:
            break;
        }
    }
    return Node::canAdoptChild(child);
}


QSet<Node*> OutEventPortImpl::getParentNodesForValidDefinition(){
    QSet<Node*> parents;
    //Need to look at The Component's defintion
    auto component = getTopBehaviourContainer();
    if(component){
        auto component_definition = component->getDefinition(true);
        if(component_definition){
            parents << component_definition;
        }
    }
    return parents;
}

Node* OutEventPortImpl::getTopBehaviourContainer(){
    if(!top_behaviour_calculated){
        top_behaviour_container = ContainerNode::getTopBehaviourContainer(this);
        top_behaviour_calculated = true;
    }
    return top_behaviour_container;
}