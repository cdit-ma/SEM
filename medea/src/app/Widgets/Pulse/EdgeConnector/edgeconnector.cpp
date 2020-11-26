//
// Created by Cathlyn Aston on 24/11/20.
//

#include "edgeconnector.h"
#include "../EdgeAnchor/naturalanchor.h"
#include "../../../theme.h"

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