//
// Created by Cathlyn Aston on 29/10/20.
//

#include "tricolumntray.h"
#include "../pulseviewutils.h"
#include "../pulseviewdefaults.h"
#include "../pulseviewutils.h"

#include <stdexcept>

using namespace Pulse::View;

const qreal vertical_spacing = Defaults::layout_margin;
const qreal horizontal_spacing = vertical_spacing * 1.5;

/**
 * @brief TriColumnTray::TriColumnTray
 * @param parent
 */
TriColumnTray::TriColumnTray(QGraphicsItem* parent)
    : QGraphicsWidget(parent) {}

/**
 * @brief TriColumnTray::addItem
 * @param widget
 */
void TriColumnTray::addItem(QGraphicsWidget* widget)
{
    addCenter(widget);
}

/**
 * @brief TriColumnTray::removeItem
 * @param widget
 */
void TriColumnTray::removeItem(QGraphicsWidget* widget)
{
    if (widget != nullptr) {
        if (grid_layout_ != nullptr) {
            prepareGeometryChange();
            grid_layout_->removeItem(widget);
            widget->setParentItem(nullptr);
            // TODO: Find out what happens to the position of the remaining widgets in the layout
            //  If they don't get adjusted automatically, ask if that will be a problem
        }
    }
}

/**
 * @brief TriColumnTray::addLeft
 * @param widget
 */
void TriColumnTray::addLeft(QGraphicsWidget* widget)
{
    addWidget(widget, left_count_++, left_column_);
}

/**
 * @brief TriColumnTray::addRight
 * @param widget
 */
void TriColumnTray::addRight(QGraphicsWidget* widget)
{
    addWidget(widget, right_count_++, right_column_);
}

/**
 * @brief TriColumnTray::addCenter
 * @param widget
 */
void TriColumnTray::addCenter(QGraphicsWidget* widget)
{
    addWidget(widget, center_count_++, center_column_);
}

/**
 * @brief TriColumnTray::isEmpty
 * @return
 */
bool TriColumnTray::isEmpty() const
{
    for (const auto child : childItems()) {
        if (child->isWidget()) {
            return false;
        }
    }
    return true;
}

/**
 * @brief TriColumnTray::boundingRect
 * @return
 */
QRectF TriColumnTray::boundingRect() const
{
    return {QPointF(0,0), Utils::getVisibleChildrenRect(this).bottomRight()};
}

/**
 * @brief TriColumnTray::setGeometry
 * @param geom
 */
void TriColumnTray::setGeometry(const QRectF& geom)
{
    prepareGeometryChange();
    QGraphicsWidget::setGeometry(QRectF(geom.topLeft(), boundingRect().size()));
}

/**
 * @brief TriColumnTray::addWidget
 * @param widget
 * @param row
 * @param column
 */
void TriColumnTray::addWidget(QGraphicsWidget* widget, int row, int column)
{
    checkPreConditions(widget);

    if (grid_layout_ == nullptr) {
        constructGridLayout();
    }

    prepareGeometryChange();
    grid_layout_->addItem(widget, row, column);
}

/**
 * @brief TriColumnTray::checkPreConditions
 * @param widget
 * @throws std::invalid_argument
 * @throws std::logic_error
 */
void TriColumnTray::checkPreConditions(QGraphicsWidget* widget) const
{
    if (widget == nullptr) {
        throw std::invalid_argument("TriColumnTray - Trying to add a null item");
    }
    if (childItems().contains(widget)) {
        throw std::logic_error("TriColumnTray - Trying to add an item that already exists");
    }
}

/**
 * @brief TriColumnTray::constructGridLayout
 */
void TriColumnTray::constructGridLayout()
{
    grid_layout_ = new QGraphicsGridLayout(this);
    grid_layout_->setContentsMargins(0, 0, 0, 0);
    grid_layout_->setVerticalSpacing(vertical_spacing);
    grid_layout_->setHorizontalSpacing(horizontal_spacing);
}