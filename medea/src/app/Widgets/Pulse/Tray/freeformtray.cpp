//
// Created by Cathlyn Aston on 29/10/20.
//

#include "freeformtray.h"
#include "../pulseviewutils.h"
#include "../pulseviewdefaults.h"
#include "../pulseviewutils.h"

#include <stdexcept>

using namespace Pulse::View;

const qreal stack_gap_ = Defaults::layout_padding * 2;

/**
 * @brief FreeFormTray::FreeFormTray
 * @param parent
 */
FreeFormTray::FreeFormTray(QGraphicsItem* parent)
    : QGraphicsWidget(parent) {}

/**
 * @brief FreeFormTray::addItem
 * @param widget
 */
void FreeFormTray::addItem(QGraphicsWidget* widget)
{
    checkPreConditions(widget);
    prepareGeometryChange();

    auto stack_pos = getNextStackPos();
    widget->setPos(stack_pos);
    widget->setParentItem(this);
    widget->setVisible(true);
    contained_items_.append(widget);

    // When a child item's geometry has changed, update the tray's geometry and schedule a repaint
    connect(widget, &QGraphicsWidget::geometryChanged, [this]() {
        updateGeometry();
        update();
    });
}

/**
 * @brief FreeFormTray::removeItem
 * @param widget
 */
void FreeFormTray::removeItem(QGraphicsWidget* widget)
{
    if (widget != nullptr) {
        prepareGeometryChange();
        widget->setParentItem(nullptr);
        disconnect(widget, nullptr, this, nullptr);
        contained_items_.removeAll(widget);
    }
}

/**
 * @brief FreeFormTray::isEmpty
 * @return
 */
bool FreeFormTray::isEmpty() const
{
    return contained_items_.empty();
}

/**
 * @brief FreeFormTray::boundingRect
 * @return
 */
QRectF FreeFormTray::boundingRect() const
{
    return {QPointF(0,0), Utils::getVisibleChildrenRect(this).bottomRight()};
}

/**
 * @brief FreeFormTray::setGeometry
 * @param geom
 */
void FreeFormTray::setGeometry(const QRectF& geom)
{
    // Force this item's geometry to have the same size as the bounding rect
    prepareGeometryChange();
    QGraphicsWidget::setGeometry(QRectF(geom.topLeft(), boundingRect().size()));
}

/**
 * @brief FreeFormTray::getNextStackPos
 * @return
 */
QPointF FreeFormTray::getNextStackPos() const
{
    const int content_count = contained_items_.size();
    return {stack_gap_ * content_count, stack_gap_ * content_count};
}

/**
 * @brief FreeFormTray::checkPreConditions
 * @param widget
 * @throws std::invalid_argument
 * @throws std::logic_error
 */
void FreeFormTray::checkPreConditions(QGraphicsWidget* widget) const
{
    if (widget == nullptr) {
        throw std::invalid_argument("FreeFormTray - Trying to add a null item");
    }
    if (contained_items_.contains(widget)) {
        throw std::logic_error("FreeFormTray - Trying to add an item that already exists");
    }
}