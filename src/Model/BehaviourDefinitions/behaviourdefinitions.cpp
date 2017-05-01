#include "behaviourdefinitions.h"
#include "../nodekinds.h"

#include "../entityfactory.h"
BehaviourDefinitions::BehaviourDefinitions(EntityFactory* factory) : Node(factory, NODE_KIND::BEHAVIOUR_DEFINITIONS, "BehaviourDefinitions"){
	auto node_kind = NODE_KIND::BEHAVIOUR_DEFINITIONS;
	QString kind_string = "BehaviourDefinitions";
	RegisterNodeKind(factory, node_kind, kind_string, [](){return new BehaviourDefinitions();});

    //Register Data
    RegisterDefaultData(factory, node_kind, "label", QVariant::String, true, "COMPONENTS");
};

BehaviourDefinitions::BehaviourDefinitions():Node(NODE_KIND::BEHAVIOUR_DEFINITIONS)
{
    setNodeType(NODE_TYPE::ASPECT);

    setMoveEnabled(false);
    setExpandEnabled(false);
    updateDefaultData("label", QVariant::String, true, "COMPONENTS");
}

VIEW_ASPECT BehaviourDefinitions::getViewAspect() const
{
    return VA_BEHAVIOUR;
}

bool BehaviourDefinitions::canAdoptChild(Node *child)
{
    switch(child->getNodeKind()){
    case NODE_KIND::COMPONENT_IMPL:{
        break;
    }
    default:
        return false;
    }
    return Node::canAdoptChild(child);
}

bool BehaviourDefinitions::canAcceptEdge(EDGE_KIND, Node *)
{
    return false;
}
