//
// Created by Cathlyn Aston on 19/11/20.
//

#include "naturalanchor.h"
#include "../pulseviewutils.h"

#include <QPainter>

using namespace Pulse::View;

/**
 * @brief NaturalAnchor::NaturalAnchor
 * @param parent
 */
NaturalAnchor::NaturalAnchor(QGraphicsItem* parent)
    : QGraphicsObject(parent),
      edge_connector_(new EdgeConnector(this))
{
    connect(this, &NaturalAnchor::visibleChanged, [this]() {
        emit edgeAnchorVisibilityChanged(isVisible());
    });
    connectEdgeConnector();
}

/**
 * @brief NaturalAnchor::getEdgeConnector
 * @throws std::runtime_error
 * @return
 */
EdgeConnector& NaturalAnchor::getEdgeConnector()
{
    if (edge_connector_ == nullptr) {
        throw std::runtime_error("NaturalAnchor::getEdgeConnector - The edge connector is null");
    }
    return *edge_connector_;
}

/*
 * @brief NaturalAnchor::transferToAdopter
 * @param adopter
 * @throws std::invalid_argument
 */
void NaturalAnchor::transferToAdopter(EdgeAdopter* adopter)
{
    if (adopter == nullptr) {
        throw std::invalid_argument("NaturalAnchor::transferToAdopter - The edge adopter is null");
    }

    // We need to update the EdgeConnector's parentItem() if we want to see it at all depth
    auto graphics_obj = dynamic_cast<QGraphicsObject*>(adopter);
    edge_connector_->setParentItem(graphics_obj);

    disconnect(this, nullptr, edge_connector_, nullptr);
    adopter->adoptEdges(this, edge_connector_);
    active_adopter_ = adopter;
}

/**
 * @brief NaturalAnchor::retrieveFromAdopter
 */
void NaturalAnchor::retrieveFromAdopter()
{
    if (active_adopter_ != nullptr) {
        active_adopter_->returnEdges(this);
        active_adopter_ = nullptr;
    }
    connectEdgeConnector();
}

/**
 * @brief NaturalAnchor::triggerPositionChange
 * @param x
 * @param y
 */
void NaturalAnchor::triggerPositionChange(qreal x, qreal y)
{
    setPos(x, y);
    emit edgeAnchorMoved(scenePos());
}

/**
 * @brief NaturalAnchor::connectEdgeConnector
 */
void NaturalAnchor::connectEdgeConnector()
{
    connect(this, &NaturalAnchor::edgeAnchorVisibilityChanged, edge_connector_, &EdgeConnector::visibilityChanged);
    connect(this, &NaturalAnchor::edgeAnchorMoved, edge_connector_, &EdgeConnector::positionChanged);
    edge_connector_->setParentItem(this);
    edge_connector_->positionChanged(scenePos());
}