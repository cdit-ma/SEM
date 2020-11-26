//
// Created by Cathlyn Aston on 24/11/20.
//

#include "edgeconnector.h"
#include "../EdgeAnchor/naturalanchor.h"
#include "../../../theme.h"

#include <stdexcept>
#include <QPainter>

using namespace Pulse::View;

const qreal size = 10;

/**
 * @brief EdgeConnector::EdgeConnector
 * @param parent
 */
EdgeConnector::EdgeConnector(QGraphicsItem* parent)
    : QGraphicsObject(parent)
{
    connect(this, &EdgeConnector::visibleChanged, [this]() {
        emit visibilityChanged(isVisible());
    });

    color_ = Theme::theme()->getTextColor();
    connect(Theme::theme(), &Theme::theme_Changed, [this]() {
        color_ = Theme::theme()->getTextColor();
        update();
    });

    // The EdgeConnector doesn't show until an edge has been connected to it
    setVisible(false);
}

/**
 * @brief EdgeConnector::connectEdge
 * @param edge
 * @throws std::invalid_argument
 */
void EdgeConnector::connectEdge(Edge* edge)
{
    if (edge == nullptr) {
        throw std::invalid_argument("EdgeConnector::connectEdge - The edge is null");
    }
    if (!connected_edges_.contains(edge)) {
        connected_edges_.append(edge);
        setVisible(true);
    }
}

/**
 * @brief EdgeConnector::disconnectEdge
 * @param edge
 * @throws std::invalid_argument
 */
void EdgeConnector::disconnectEdge(Edge* edge)
{
    if (edge == nullptr) {
        throw std::invalid_argument("EdgeConnector::disconnectEdge");
    }
    connected_edges_.removeAll(edge);
    if (connected_edges_.isEmpty()) {
        setVisible(false);
    }
}

/**
 * @brief EdgeConnector::setNaturalAnchor
 * @param anchor
 * @throws std::invalid_argument
 */
void EdgeConnector::setNaturalAnchor(NaturalAnchor* anchor)
{
    if (anchor == nullptr) {
        throw std::invalid_argument("EdgeConnector::setNaturalAnchor - The anchor is null");
    }
    natural_anchor_ = anchor;
    connect(natural_anchor_, &NaturalAnchor::destroyed, this, &EdgeConnector::deleteLater);
}

/**
 * @brief EdgeConnector::isConnectedToNaturalAnchor()
 * @return
 */
bool EdgeConnector::isConnectedToNaturalAnchor() const
{
    return parentItem() == natural_anchor_;
}

/**
 * @brief EdgeConnector::setNaturalAnchor
 * @param anchor
 */
void EdgeConnector::setNaturalAnchor(NaturalAnchor* anchor)
{
    if (anchor == nullptr) {
        throw std::invalid_argument("EdgeConnector::setNaturalAnchor - The anchor is null");
    }
    natural_anchor_ = anchor;
    connect(natural_anchor_, &NaturalAnchor::destroyed, this, &EdgeConnector::deleteLater);
}

/**
 * @brief EdgeConnector::isConnectedToNaturalAnchor()
 * @return
 */
bool EdgeConnector::isConnectedToNaturalAnchor() const
{
    return parentItem() == natural_anchor_;
}

/**
 * @brief EdgeConnector::paint
 * @param painter
 * @param option
 * @param widget
 */
void EdgeConnector::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    painter->setPen(Qt::NoPen);
    painter->setBrush(color_);
    painter->drawEllipse(boundingRect());
}

/**
 * @brief EdgeConnector::boundingRect
 * @return
 */
QRectF EdgeConnector::boundingRect() const
{
    auto half_size = size / 2.0;
    return QRectF(-half_size, -half_size, size, size);
}