//
// Created by Cathlyn Aston on 23/11/20.
//

#ifndef PULSE_VIEW_UTILS_H
#define PULSE_VIEW_UTILS_H

#include "Entity/entity.h"
#include "../../Widgets/Charts/Data/protomessagestructs.h"

#include <QGraphicsWidget>
#include <stdexcept>

namespace Pulse::View::Utils {

/**
 * @brief Pulse::View::Utils::getEntityAsGraphicsWidget
 * @param entity
 * @throws std::invalid_argument
 * @throws std::runtime_error
 * @return
 */
static QGraphicsWidget* getEntityAsGraphicsWidget(Entity* entity)
{
    if (entity == nullptr) {
        throw std::invalid_argument("Pulse::View::Utils::getEntityAsGraphicsWidget - The entity is null");
    }
    auto widget = entity->getAsGraphicsWidget();
    if (widget == nullptr) {
        throw std::runtime_error("Pulse::View::Utils::getEntityAsGraphicsWidget - The entity is not a QGraphicsWidget");
    }
    return widget;
}

static QGraphicsWidget* getTopmostParentWidget(const QGraphicsWidget* widget)
{
    if (widget == nullptr) {
        throw std::invalid_argument("Pulse::View::Utils::getTopmostParentWidget - The graphics item is null");
    }
    auto parent = widget->parentWidget();
    while (parent != nullptr) {
        parent = parent->parentWidget();
    }
    return parent;
}

static int getDepth(const QGraphicsWidget* widget)
{
    if (widget == nullptr) {
        throw std::invalid_argument("Pulse::View::Utils::getDepth - The graphics item is null");
    }
    int depth = 0;
    auto parent = widget->parentWidget();
    while (parent != nullptr) {
        parent = parent->parentWidget();
        depth++;
    }
    return depth;
}

static QRectF getVisibleChildrenRect(const QGraphicsWidget* widget)
{
    if (widget == nullptr) {
        throw std::invalid_argument("Pulse::View::Utils::getVisibleItemsRect - The graphics item is null");
    }
    QRectF visible_rect;
    for (const auto& child_item : widget->childItems()) {
        if (child_item->isVisible()) {
            auto&& child_geom = QRectF(child_item->pos(), child_item->boundingRect().size());
            visible_rect = visible_rect.united(child_geom);
        }
    }
    return visible_rect;
}

static QString getIconName(AggServerResponse::Port::Kind kind)
{
    switch (kind) {
        case AggServerResponse::Port::Kind::PERIODIC:
            return "clockCycle";
        case AggServerResponse::Port::Kind::PUBLISHER:
            return "arrowRightLong";
        case AggServerResponse::Port::Kind::SUBSCRIBER:
            return "arrowIntoBox";
        case AggServerResponse::Port::Kind::REQUESTER:
            return "arrowTopRight";
        case AggServerResponse::Port::Kind::REPLIER:
            return "arrowBottomRight";
        case AggServerResponse::Port::Kind::NO_KIND:
            throw std::invalid_argument("Pulse::View::Utils::getIconName - Port kind is unknown");
    }
}

} // end Pulse::View namespace
#endif // PULSE_VIEW_UTILS_H
