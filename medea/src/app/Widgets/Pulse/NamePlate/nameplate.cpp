//
// Created by Cathlyn Aston on 24/9/20.
//

#include "nameplate.h"
#include <QGraphicsLinearLayout>

using namespace Pulse::View;

const int pixmap_size = 50;
const int pixmap_padding = 10;
const int meta_pixmap_size = pixmap_size / 2.5;

const QString font_family = "Verdana";
const int font_point_size = 14;
const int meta_font_point_size = 8;

const int main_layout_spacing = 5;
const int layout_spacing = 2;
const int layout_horizontal_margin = 5;

NamePlate::NamePlate(const QString& label,
                     const QString& icon_name,
                     const QString& meta_label,
                     const QString& meta_icon_name,
                     QGraphicsItem* parent)
    : QGraphicsWidget(parent),
      text_item_(new TextItem(label, this)),
      pixmap_item_(new PixmapItem(icon_name, this)),
      meta_text_item_(new TextItem(meta_label, this)),
      meta_pixmap_item_(new PixmapItem(meta_icon_name, this))
{
    pixmap_item_->setPixmapPadding(pixmap_padding);
    pixmap_item_->setPixmapSize(pixmap_size, pixmap_size);
    meta_pixmap_item_->setPixmapSize(meta_pixmap_size, meta_pixmap_size);

    text_item_->setFont(QFont(font_family, font_point_size));
    meta_text_item_->setFont(QFont(font_family, meta_font_point_size));

    auto meta_layout = new QGraphicsLinearLayout(Qt::Horizontal);
    meta_layout->setSpacing(layout_spacing);
    meta_layout->setContentsMargins(0, 0, 0, 0);
    meta_layout->addItem(meta_pixmap_item_);
    meta_layout->addItem(meta_text_item_);
    meta_layout->setAlignment(meta_text_item_, Qt::AlignLeft);
    meta_layout->setStretchFactor(meta_text_item_, 1);

    auto info_layout = new QGraphicsLinearLayout(Qt::Vertical);
    info_layout->setSpacing(layout_spacing);
    info_layout->setContentsMargins(0, 0, 0, 0);
    info_layout->addItem(text_item_);
    info_layout->addItem(meta_layout);

    auto main_layout = new QGraphicsLinearLayout(Qt::Horizontal, this);
    main_layout->setSpacing(main_layout_spacing);
    main_layout->setContentsMargins(layout_horizontal_margin, 0, layout_horizontal_margin, 0);
    main_layout->addItem(pixmap_item_);
    main_layout->addItem(info_layout);
    main_layout->setAlignment(info_layout, Qt::AlignLeft);
    main_layout->setStretchFactor(info_layout, 1);

    setContentsMargins(0, 0, 0, 0);
    setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);
}

void NamePlate::changeIcon(const QString& icon_name)
{
    pixmap_item_->setPixmap(icon_name);
}

void NamePlate::changeLabel(const QString& label)
{
    prepareGeometryChange();
    text_item_->setText(label);
    updateGeometry();
}
