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

void FreeFormTray::addItem(QGraphicsWidget* widget)
{
    checkPreCondition(widget);
    prepareGeometryChange();

    auto stack_pos = getNextStackPos();
    widget->setPos(stack_pos);
    widget->setParentItem(this);
    contained_items_.push_back(widget);
}

QRectF FreeFormTray::boundingRect() const
{
    QRectF rect(QPointF(0,0), getVisibleItemsRect().bottomRight());
    return rect.adjusted(0, 0, tray_padding, tray_padding);
}

void FreeFormTray::validateItemMove(QGraphicsWidget* widget, const QPointF& pos)
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

QPointF FreeFormTray::getContentOrigin()
{
    return {tray_padding, tray_padding};
}

QPointF FreeFormTray::getNextStackPos() const
{
    const int content_count = contained_items_.size();
    QPointF offset(stack_gap_ * content_count, stack_gap_ * content_count);
    return getContentOrigin() + offset;
}

QRectF FreeFormTray::getVisibleItemsRect() const
{
    QRectF visible_rect;
    for (const auto& child_item : childItems()) {
        if (child_item->isVisible()) {
            auto&& child_geom = QRectF(child_item->pos(), child_item->boundingRect().size());
            visible_rect = visible_rect.united(child_geom);
        }
    }
    return visible_rect;
}

void FreeFormTray::checkPreCondition(QGraphicsWidget* widget) const
{
    if (widget == nullptr) {
        throw std::invalid_argument("FreeFormTray - Trying to add a null item");
    }
    if (childItems().contains(widget)) {
        throw std::logic_error("FreeFormTray - Trying to add an item that already exists");
    }
}
