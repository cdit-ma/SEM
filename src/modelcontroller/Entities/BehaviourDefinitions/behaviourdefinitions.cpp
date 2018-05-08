#include "behaviourdefinitions.h"
#include "../../entityfactory.h"

const NODE_KIND node_kind = NODE_KIND::BEHAVIOUR_DEFINITIONS;
const QString kind_string = "BehaviourDefinitions";

void BehaviourDefinitions::RegisterWithEntityFactory(EntityFactory& factory){
    Node::RegisterWithEntityFactory(factory, node_kind, kind_string, [](EntityFactory& factory, bool is_temp_node){
        return new BehaviourDefinitions(factory, is_temp_node);
        });
}

BehaviourDefinitions::BehaviourDefinitions(EntityFactory& factory, bool is_temp) : Node(factory, node_kind, is_temp){
    if(is_temp){
        return;
    }
    
    //Setup State
    setNodeType(NODE_TYPE::ASPECT);
    setAcceptsNodeKind(NODE_KIND::CLASS);
    setAcceptsNodeKind(NODE_KIND::COMPONENT_IMPL);

    //Setup Data
    factory.AttachData(this, "label", QVariant::String, "BEHAVIOUR", true);
}

VIEW_ASPECT BehaviourDefinitions::getViewAspect() const
{
    return VIEW_ASPECT::BEHAVIOUR;
}