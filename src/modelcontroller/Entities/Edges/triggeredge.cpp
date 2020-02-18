//
// Created by Cathlyn on 13/02/2020.
//

#include "triggeredge.h"
#include "../node.h"
#include "../../entityfactorybroker.h"
#include "../../entityfactoryregistrybroker.h"

const auto edge_kind = EDGE_KIND::TRIGGER;
const QString kind_string = "Edge_Trigger";

/**
 * @brief TriggerEdge::RegisterWithEntityFactory
 * @param registry_broker
 */
void TriggerEdge::RegisterWithEntityFactory(EntityFactoryRegistryBroker& registry_broker)
{
    registry_broker.RegisterWithEntityFactory(edge_kind, kind_string, [](EntityFactoryBroker& broker, Node* src, Node* dst){
        return new TriggerEdge(broker, src, dst);
    });
}

/**
 * @brief TriggerEdge::TriggerEdge
 * @param factory_broker
 * @param src
 * @param dst
 * @throws std::invalid_argument
 */
TriggerEdge::TriggerEdge(EntityFactoryBroker& factory_broker, Node* src, Node* dst)
    : Edge(factory_broker, src, dst, edge_kind)
{
    if (src != nullptr && dst != nullptr) {
        auto&& src_kind = src->getNodeKind();
        auto&& dst_kind = dst->getNodeKind();
        if (src_kind == NODE_KIND::STRATEGY_INST && dst_kind == NODE_KIND::DEPLOYMENT_CONTAINER) {
            // Set the src node's Container_reference data
            src->setDataValue(KeyName::ContainerReference, dst->getID());
        } else {
            throw std::invalid_argument("TriggerEdge::TriggerEdge - Cannot construct edge; invalid src and/or dst kind");
        }
    } else {
        throw std::invalid_argument("TriggerEdge::TriggerEdge - Cannot construct edge; src and/or dst node is null");
    }
}