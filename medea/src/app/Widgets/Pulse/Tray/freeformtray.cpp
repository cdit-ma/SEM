//
// Created by Cathlyn Aston on 29/10/20.
//

#include "freeformtray.h"
#include "../pulseviewdefaults.h"

using namespace Pulse::View;

const qreal tray_padding = Defaults::big_padding;
const qreal stack_gap_ = Defaults::minimum_height / 2.0;

FreeFormTray::FreeFormTray(QGraphicsItem* parent)
    : QGraphicsWidget(parent) {}

void FreeFormTray::add(QGraphicsWidget* widget)
{
    if (widget != nullptr) {
        prepareGeometryChange();
        auto stack_pos = getContentStackPos();
        widget->setPos(stack_pos);
        widget->setParentItem(this);
        content_widgets_.push_back(widget);
    }
}

QRectF FreeFormTray::boundingRect() const
{
    QRectF rect(QPointF(0,0), childrenBoundingRect().bottomRight());
    return rect.adjusted(0, 0, tray_padding, tray_padding);
}

QPointF FreeFormTray::getContentOrigin()
{
    return {tray_padding, tray_padding};
}

QPointF FreeFormTray::getContentStackPos() const
{
    const int content_count = content_widgets_.size();
    QPointF offset(stack_gap_ * content_count, stack_gap_ * content_count);
    return getContentOrigin() + offset;
}

void FreeFormTray::validateContentMove(QGraphicsWidget* widget, const QPointF& pos)
{
    if (widget == nullptr) {
        return;
    }

    auto&& min_x = getContentOrigin().x();
    auto&& min_y = getContentOrigin().y();
    auto x = pos.x();
    auto y = pos.y();

    // This prevents a left/upward drag
    if (x < min_x) {
        x = min_x;
    }
    if (y < min_y) {
        y = min_y;
    }

    prepareGeometryChange();
    widget->setPos(x, y);
}
