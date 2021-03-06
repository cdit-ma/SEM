//
// Created by Cathlyn on 6/02/2020.
//

#include "triggerinst.h"
#include "../../entityfactorybroker.h"
#include "../../entityfactoryregistrybroker.h"

const NODE_KIND node_kind = NODE_KIND::TRIGGER_INST;
const QString kind_string = "Trigger Instance";

/**
 * @brief TriggerInst::RegisterWithEntityFactory
 * @param registry_broker
 */
void TriggerInst::RegisterWithEntityFactory(EntityFactoryRegistryBroker& registry_broker)
{
    registry_broker.RegisterWithEntityFactory(node_kind, kind_string, [](EntityFactoryBroker& factory_broker, bool is_temp_node) {
        return new TriggerInst(factory_broker, is_temp_node);
    });
}

/**
 * @brief TriggerInst::TriggerInst
 * @param factory_broker
 * @param is_temp_node
 */
TriggerInst::TriggerInst(EntityFactoryBroker& factory_broker, bool is_temp_node)
        : Node(factory_broker, node_kind, is_temp_node)
{
    addInstancesDefinitionKind(NODE_KIND::TRIGGER_DEFN);
 
    // Set NODE_KIND-specific ruling for adoption and connections
    SetEdgeRuleActive(EdgeRule::ALLOW_EXTERNAL_DEFINITIONS);
    setAcceptsNodeKind(NODE_KIND::STRATEGY_INST);

    // Break out early for temporary entities
    if (is_temp_node) {
        return;
    }

    // Setup data
    factory_broker.AttachData(this, KeyName::Type, QVariant::String, ProtectedState::PROTECTED);
    factory_broker.AttachData(this, KeyName::Row, QVariant::Int, ProtectedState::PROTECTED, 1);
    factory_broker.AttachData(this, KeyName::Column, QVariant::Int, ProtectedState::PROTECTED, 3);
    
    // Construct child StrategyInst
    factory_broker.ConstructChildNode(*this, NODE_KIND::STRATEGY_INST);
}