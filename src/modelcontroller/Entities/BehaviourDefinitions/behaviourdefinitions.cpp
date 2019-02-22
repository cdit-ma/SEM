#include "behaviourdefinitions.h"
#include "../../entityfactorybroker.h"
#include "../../entityfactoryregistrybroker.h"

const NODE_KIND node_kind = NODE_KIND::BEHAVIOUR_DEFINITIONS;
const QString kind_string = "Behaviour Definitions";

void BehaviourDefinitions::RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker){
    broker.RegisterWithEntityFactory(node_kind, kind_string, [](EntityFactoryBroker& broker, bool is_temp_node){
        return new BehaviourDefinitions(broker, is_temp_node);
        });
}

BehaviourDefinitions::BehaviourDefinitions(EntityFactoryBroker& broker, bool is_temp) : Node(broker, node_kind, is_temp){
    //Setup State
    setNodeType(NODE_TYPE::ASPECT);
    setAcceptsNodeKind(NODE_KIND::CLASS);
    setAcceptsNodeKind(NODE_KIND::COMPONENT_IMPL);
    setAcceptsNodeKind(NODE_KIND::NAMESPACE);

    if(is_temp){
        //Break out early for temporary entities
        return;
    }

    //Setup Data
    setLabelFunctional(false);
    broker.AttachData(this, "label", QVariant::String, ProtectedState::PROTECTED, "BEHAVIOUR");
}

VIEW_ASPECT BehaviourDefinitions::getViewAspect() const
{
    return VIEW_ASPECT::BEHAVIOUR;
}
