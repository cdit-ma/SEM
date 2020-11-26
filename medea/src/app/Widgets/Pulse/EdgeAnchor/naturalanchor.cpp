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
    connect(this, &NaturalAnchor::visibleChanged, [this] () {
        emit edgeAnchorVisibilityChanged(isVisible());
    });
    edge_connector_->setNaturalAnchor(this);
    connectEdgeConnector();
}

/**
 * @brief NaturalAnchor::getEdgeConnector
 * @throws std::runtime_error
 * @return
 */
EdgeConnector& NaturalAnchor::getEdgeConnector() const
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

    // Transfer the parent-ship of the EdgeConnector to the adopter
    auto graphics_obj = dynamic_cast<QGraphicsObject*>(adopter);
    getEdgeConnector().setParentItem(graphics_obj);

    disconnect(this, nullptr, &getEdgeConnector(), nullptr);
    adopter->adoptEdges(this, &getEdgeConnector());
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
    connect(this, &NaturalAnchor::edgeAnchorMoved, &getEdgeConnector(), &EdgeConnector::positionChanged);
    getEdgeConnector().setParentItem(this);
    getEdgeConnector().visibilityChanged(isVisible());
    getEdgeConnector().positionChanged(scenePos());
}