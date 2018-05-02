#include "behaviourdefinitions.h"

#include "../../edgekinds.h"
#include "../../edgekinds.h"


BehaviourDefinitions::BehaviourDefinitions(EntityFactory* factory) : Node(factory, NODE_KIND::BEHAVIOUR_DEFINITIONS, "BehaviourDefinitions"){
	auto node_kind = NODE_KIND::BEHAVIOUR_DEFINITIONS;
	QString kind_string = "BehaviourDefinitions";
	RegisterNodeKind(factory, node_kind, kind_string, [](){return new BehaviourDefinitions();});

    //Register Data
    RegisterDefaultData(factory, node_kind, "label", QVariant::String, true, "BEHAVIOUR");
};

BehaviourDefinitions::BehaviourDefinitions():Node(NODE_KIND::BEHAVIOUR_DEFINITIONS)
{
    setNodeType(NODE_TYPE::ASPECT);

    setAcceptsNodeKind(NODE_KIND::CLASS);
    setAcceptsNodeKind(NODE_KIND::COMPONENT_IMPL);
}

VIEW_ASPECT BehaviourDefinitions::getViewAspect() const
{
    return VIEW_ASPECT::BEHAVIOUR;
}