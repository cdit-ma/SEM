//
// Created by Cathlyn Aston on 29/10/20.
//

#include "tricolumntray.h"
#include "../pulseviewdefaults.h"

using namespace Pulse::View;

const qreal tray_padding = Defaults::big_padding;
const qreal horizontal_spacing = Defaults::minimum_height / 2.0;

TriColumnTray::TriColumnTray(QGraphicsItem* parent)
    : QGraphicsWidget(parent)
{
    grid_layout_ = new QGraphicsGridLayout(this);
    grid_layout_->setContentsMargins(tray_padding, tray_padding, tray_padding, tray_padding);
    grid_layout_->setSpacing(tray_padding);
    grid_layout_->setVerticalSpacing(tray_padding);
    grid_layout_->setHorizontalSpacing(horizontal_spacing);
}

void TriColumnTray::addItem(QGraphicsWidget* widget)
{
    addCenter(widget);
}

void TriColumnTray::addLeft(QGraphicsWidget* widget)
{
    checkPreCondition(widget);
    prepareGeometryChange();
    grid_layout_->addItem(widget, left_count_++, left_column_);
}

void TriColumnTray::addRight(QGraphicsWidget* widget)
{
    checkPreCondition(widget);
    prepareGeometryChange();
    grid_layout_->addItem(widget, right_count_++, right_column_);
}

void TriColumnTray::addCenter(QGraphicsWidget* widget)
{
    checkPreCondition(widget);
    prepareGeometryChange();
    grid_layout_->addItem(widget, center_count_++, center_column_);
}

void TriColumnTray::checkPreCondition(QGraphicsWidget* widget) const
{
    if (widget == nullptr) {
        throw std::invalid_argument("TriColumnTray - Trying to add a null item");
    }
    if (childItems().contains(widget)) {
        throw std::logic_error("TriColumnTray - Trying to add an item that already exists");
    }
}
