//
// Created by Cathlyn Aston on 19/11/20.
//

#include "delegateanchor.h"
#include <stdexcept>

using namespace Pulse::View;

/**
 * @brief DelegateAnchor::DelegateAnchor
 * @param parent
 */
DelegateAnchor::DelegateAnchor(QGraphicsItem* parent)
    : QGraphicsObject(parent)
{
    connect(this, &DelegateAnchor::visibleChanged, [this] () {
        emit visibilityChanged(isVisible());
    });
}

/**
 * @brief DelegateAnchor::transferToAdopter
 * @param adopter
 * @throws std::invalid_argument
 */
void DelegateAnchor::transferToAdopter(EdgeAdopter* adopter)
{
    if (adopter == nullptr) {
        throw std::invalid_argument("DelegateAnchor::transferToAdopter - The edge adopter is null");
    }

    // Disconnect the signals to all attached EdgeConnectors before transferring them to the EdgeAdopter
    disconnectEdgeConnectors();

    // Transfer the parent-ship of the attached EdgeConnectors to the adopter
    auto graphics_obj = dynamic_cast<QGraphicsObject*>(adopter);
    for (auto connector : attached_connectors_) {
        connector->setParentItem(graphics_obj);
        adopter->adoptEdges(this, connector);
    }

    active_adopter_ = adopter;
}

/**
 * @brief DelegateAnchor::retrieveFromAdopter
 */
void DelegateAnchor::retrieveFromAdopter()
{
    if (active_adopter_ != nullptr) {
        active_adopter_->returnEdges(*this);
        active_adopter_ = nullptr;
    }
    // Reconnect the signals to all attached EdgeConnectors
    reconnectEdgeConnectors();
}

/**
 * @brief DelegateAnchor::adoptEdges
 * @param anchor
 * @param connector
 * @throws std::invalid_argument
 * @throws std::logic_error
 */
void DelegateAnchor::adoptEdges(EdgeAnchor* anchor, EdgeConnector* connector)
{
    if (anchor == nullptr) {
        throw std::invalid_argument("DelegateAnchor::adoptEdges - The edge anchor is null");
    }
    if (connector == nullptr) {
        throw std::invalid_argument("DelegateAnchor::adoptEdges - The edge connector is null");
    }
    const auto& anchor_connectors = attached_connectors_.values(anchor);
    if (anchor_connectors.contains(connector)) {
        throw std::logic_error("DelegateAnchor::adoptEdges - The edge connector is already attached");
    }
    attached_connectors_.insert(anchor, connector);
    connectEdgeConnector(connector);
}

/**
 * @brief DelegateAnchor::returnEdges
 * @param anchor
 */
void DelegateAnchor::returnEdges(EdgeAnchor& anchor)
{
    const auto& anchor_connectors = attached_connectors_.values(&anchor);
    for (auto connector : anchor_connectors) {
        disconnectEdgeConnector(connector);
    }
    attached_connectors_.remove(&anchor);
}

/**
 * @brief DelegateAnchor::triggerPositionChange
 * @param x
 * @param y
 */
void DelegateAnchor::triggerPositionChange(qreal x, qreal y)
{
    setPos(x, y);
    emit positionChanged(scenePos());
}

/**
 * @brief DelegateAnchor::connectEdgeConnector
 * @param connector
 * @throws std::invalid_argument
 */
void DelegateAnchor::connectEdgeConnector(EdgeConnector* connector)
{
    if (connector == nullptr) {
        throw std::invalid_argument("DelegateAnchor::connectEdgeConnector - The edge connector is null");
    }
    connect(this, &DelegateAnchor::positionChanged, connector, &EdgeConnector::positionChanged);
    connector->setParentItem(this);
    connector->visibilityChanged(isVisible());
    connector->positionChanged(scenePos());
}

/**
 * @brief DelegateAnchor::disconnectEdgeConnector
 * @param connector
 */
void DelegateAnchor::disconnectEdgeConnector(EdgeConnector* connector)
{
    if (connector != nullptr) {
        disconnect(connector);
    }
}

/**
 * @brief DelegateAnchor::disconnectEdgeConnectors
 */
void DelegateAnchor::disconnectEdgeConnectors()
{
    for (auto connector : attached_connectors_) {
        disconnectEdgeConnector(connector);
    }
}

/**
 * @brief DelegateAnchor::reconnectEdgeConnectors
 */
void DelegateAnchor::reconnectEdgeConnectors()
{
    for (auto connector : attached_connectors_) {
        connectEdgeConnector(connector);
    }
}