//
// Created by Cathlyn Aston on 23/11/20.
//

#ifndef PULSE_VIEW_UTILS_H
#define PULSE_VIEW_UTILS_H

#include "Entity/entity.h"
#include "EntityContainer/entitycontainer.h"
#include "../../Widgets/Charts/Data/protomessagestructs.h"
#include "../../theme.h"

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

/**
 * @brief Pulse::View::Utils::getTopmostParentWidget
 * @param widget
 * @throws std::invalid_argument
 * @return
 */
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

/**
 * @brief Pulse::View::Utils::getDepth
 * @param widget
 * @throws std::invalid_argument
 * @return
 */
static int getDepth(const QGraphicsWidget* widget)
{
    if (widget == nullptr) {
        throw std::invalid_argument("Pulse::View::Utils::getDepth - The graphics item is null");
    }
    int depth = 0;
    auto parent = widget->parentWidget();
    while (parent != nullptr) {
        auto container = dynamic_cast<EntityContainer*>(parent);
        if (container != nullptr) {
            depth++;
        }
        parent = parent->parentWidget();
    }
    return depth;
}

/**
 * @brief Pulse::View::Utils::getTrayColor
 * @param widget
 * @param default_color
 * @param dark_theme
 * @throws std::invalid_argument
 * @return
 */
static QColor getTrayColor(const QGraphicsWidget* widget, const QColor& default_color, bool dark_theme)
{
    if (widget == nullptr) {
        throw std::invalid_argument("Pulse::View::Utils::getTrayColor - The graphics item is null");
    }
    int level = getDepth(widget) + 1;
    if (dark_theme) {
        return default_color.lighter(100 + 20 * level);
    } else {
        return default_color.lighter(100 + 5 * level);
    }
}

/**
 * @brief Pulse::View::Utils::getVisibleChildrenRect
 * @param widget
 * @throws std::invalid_argument
 * @return
 */
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

/**
 * @brief Pulse::View::Utils::getPortIconName
 * @param kind
 * @throws std::invalid_argument
 * @return
 */
static QString getPortIconName(AggServerResponse::Port::Kind kind)
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

/**
 * @brief Pulse::View::Utils::scaledPixmap
 * @param path
 * @param name
 * @param size
 * @return
 */
static QPixmap scaledPixmap(const QString& path, const QString& name, const QSize& size)
{
    auto pixmap = Theme::theme()->getImage(path, name,QSize(), Theme::theme()->getMenuIconColor());
    return pixmap.scaled(size, Qt::KeepAspectRatio, Qt::SmoothTransformation);
}

} // end Pulse::View namespace
#endif // PULSE_VIEW_UTILS_H