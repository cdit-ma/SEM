//
// Created by Cathlyn on 11/02/2020.
//

#include "triggerdefinitions.h"
#include "../../entityfactorybroker.h"
#include "../../entityfactoryregistrybroker.h"

const NODE_KIND node_kind = NODE_KIND::TRIGGER_DEFINITIONS;
const QString kind_string = "Trigger Definitions";

/**
 * @brief TriggerDefinitions::RegisterWithEntityFactory
 * @param registry_broker
 */
void TriggerDefinitions::RegisterWithEntityFactory(EntityFactoryRegistryBroker& registry_broker)
{
    registry_broker.RegisterWithEntityFactory(node_kind, kind_string, [](EntityFactoryBroker& broker, bool is_temp_node) {
        return new TriggerDefinitions(broker, is_temp_node);
    });
}


/**
 * @brief TriggerDefinitions::getViewAspect
 * @return
 */
VIEW_ASPECT TriggerDefinitions::getViewAspect() const
{
    return VIEW_ASPECT::TRIGGERS;
}


/**
 * @brief TriggerDefinitions::TriggerDefinitions
 * @param factory_broker
 * @param is_temp
 */
TriggerDefinitions::TriggerDefinitions(EntityFactoryBroker& factory_broker, bool is_temp)
    : Node(factory_broker, node_kind, is_temp)
{
    // Set ASPECT-specific states and ruling for adoption
    setNodeType(NODE_TYPE::ASPECT);
    setAcceptsNodeKind(NODE_KIND::TRIGGER_DEFN);
    
    // Break out early for temporary entities
    if (is_temp) {
        return;
    }
    
    // Setup data
    factory_broker.AttachData(this, "label", QVariant::String, ProtectedState::PROTECTED, "TRIGGERS");
    setLabelFunctional(false);
}