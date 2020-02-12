//
// Created by Cathlyn on 6/02/2020.
//

#include "trigger.h"
#include "../../entityfactorybroker.h"
#include "../../entityfactoryregistrybroker.h"

const NODE_KIND node_kind = NODE_KIND::TRIGGER_DEFN;
const QString kind_string = "Trigger";

/**
 * @brief Trigger::RegisterWithEntityFactory
 * @param registry_broker
 */
void Trigger::RegisterWithEntityFactory(EntityFactoryRegistryBroker& registry_broker)
{
    registry_broker.RegisterWithEntityFactory(node_kind, kind_string, [](EntityFactoryBroker& factory_broker, bool is_temp_node) {
        return new Trigger(factory_broker, is_temp_node);
    });
}


/**
 * @brief Trigger::getViewAspect
 * @return
 */
VIEW_ASPECT Trigger::getViewAspect() const
{
    return VIEW_ASPECT ::TRIGGERS;
}


/**
 * @brief Trigger::Trigger
 * @param factory_broker
 * @param is_temp_node
 */
Trigger::Trigger(EntityFactoryBroker& factory_broker, bool is_temp_node)
    : Node(factory_broker, node_kind, is_temp_node)
{
    addInstanceKind(NODE_KIND::TRIGGER_INST);

    // Break out early for temporary entities
    if (is_temp_node) {
        return;
    }

    // Setup data
    auto trigger_type_data = factory_broker.AttachData(this, getTableKeyString(TableKey::Type), QVariant::String, ProtectedState::UNPROTECTED);
    for (const auto& t : getTriggerTypes()) {
        trigger_type_data->addValidValue(t);
    }
    
    auto condition_data = factory_broker.AttachData(this, getTableKeyString(TableKey::Condition), QVariant::String, ProtectedState::UNPROTECTED);
    for (const auto& c : getTriggerConditions()) {
        condition_data->addValidValue(c);
    }
    
    factory_broker.AttachData(this, getTableKeyString(TableKey::Value), QVariant::Double, ProtectedState::UNPROTECTED, 0.0);
    factory_broker.AttachData(this, getTableKeyString(TableKey::SingleActivation), QVariant::Bool, ProtectedState::UNPROTECTED, false);
    factory_broker.AttachData(this, getTableKeyString(TableKey::WaitPeriod), QVariant::UInt, ProtectedState::UNPROTECTED, 1000);
}

