//
// Created by Cathlyn Aston on 24/11/20.
//

#include "edgeconnector.h"
#include "../../../theme.h"

#include <QPainter>

using namespace Pulse::View;

const qreal size = 8;

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