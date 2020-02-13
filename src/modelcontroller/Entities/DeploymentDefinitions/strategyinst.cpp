//
// Created by Cathlyn on 13/02/2020.
//

#include "strategyinst.h"
#include "../../entityfactorybroker.h"
#include "../../entityfactoryregistrybroker.h"

const NODE_KIND node_kind = NODE_KIND::STRATEGY_INST;
const QString kind_string = "Strategy Instance";

/**
 * @brief StrategyInst::RegisterWithEntityFactory
 * @param registry_broker
 */
void StrategyInst::RegisterWithEntityFactory(EntityFactoryRegistryBroker& registry_broker)
{
    registry_broker.RegisterWithEntityFactory(node_kind, kind_string, [](EntityFactoryBroker& factory_broker, bool is_temp_node) {
        return new StrategyInst(factory_broker, is_temp_node);
    });
}

/**
 * @brief StrategyInst::TriggerInst
 * @param factory_broker
 * @param is_temp_node
 */
StrategyInst::StrategyInst(EntityFactoryBroker& factory_broker, bool is_temp_node)
        : Node(factory_broker, node_kind, is_temp_node)
{
    SetEdgeRuleActive(EdgeRule::ALLOW_EXTERNAL_DEFINITIONS);
    SetEdgeRuleActive(EdgeRule::IGNORE_REQUIRED_INSTANCE_DEFINITIONS);
    setAcceptsEdgeKind(EDGE_KIND::TRIGGER, EDGE_DIRECTION::SOURCE);
    
    // Break out early for temporary entities
    if (is_temp_node) {
        return;
    }
    
    // Setup data
    factory_broker.AttachData(this, "label", QVariant::String, ProtectedState::PROTECTED);
    factory_broker.AttachData(this, "index", QVariant::Int, ProtectedState::PROTECTED);
    factory_broker.AttachData(this, "Container_reference", QVariant::String, ProtectedState::PROTECTED);
}