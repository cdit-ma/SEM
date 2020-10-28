//
// Created by Cathlyn Aston on 24/9/20.
//

#include "nameplate.h"
#include "../pulseviewdefaults.h"
#include "../../../theme.h"

using namespace Pulse::View;

/**
 * @brief NamePlate::NamePlate
 * @param label
 * @param icon_path
 * @param icon_name
 * @param meta_label
 * @param meta_icon_path
 * @param meta_icon_name
 * @param parent
 */
NamePlate::NamePlate(const QString& label,
                     const QString& icon_path,
                     const QString& icon_name,
                     const QString& meta_label,
                     const QString& meta_icon_path,
                     const QString& meta_icon_name,
                     QGraphicsItem* parent)
    : QGraphicsWidget(parent),
      primary_text_item_(new TextItem(label, this)),
      primary_pixmap_item_(new PixmapItem(QPixmap(), this)),
      secondary_text_item_(new TextItem(meta_label, this)),
      secondary_pixmap_item_(new PixmapItem(QPixmap(), this))
{
    primary_icon_ = {icon_path, icon_name};
    secondary_icon_ = {meta_icon_path, meta_icon_name};

    primary_text_item_->setFont(Defaults::primary_font);
    secondary_text_item_->setFont(Defaults::secondary_font);

    primary_pixmap_item_->setMaximumSize(Defaults::primary_icon_size);
    secondary_pixmap_item_->setMaximumSize(Defaults::secondary_icon_size);

    setContentsMargins(0, 0, 0, 0);
    setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);
    setupLayout();

    connect(Theme::theme(), &Theme::theme_Changed, this, &NamePlate::themeChanged);
    themeChanged();
}

/**
 * @brief NamePlate::changeIcon
 * @param icon_path
 * @param icon_name
 */
void NamePlate::changeIcon(const QString& icon_path, const QString& icon_name)
{
    primary_icon_ = {icon_path, icon_name};
    primary_pixmap_item_->setPixmap(scaledPixmap(icon_path, icon_name, Defaults::primary_icon_size));
}

/**
 * @brief NamePlate::changeLabel
 * @param label
 */
void NamePlate::changeLabel(const QString& label)
{
    prepareGeometryChange();
    primary_text_item_->setPlainText(label);
    updateGeometry();
}

/**
 *
 * @param left
 */
void NamePlate::setIconPos(IconPos pos)
{
    if (pos == current_icon_pos_) {
        return;
    }

    // Remove them from the layout before re-ordering them
    main_layout_->removeItem(primary_pixmap_item_);
    main_layout_->removeItem(info_layout_);

    // Re-add the items in order
    if (pos == IconPos::Left) {
        main_layout_->insertItem(0, info_layout_);
        main_layout_->insertItem(0, primary_pixmap_item_);
    } else {
        main_layout_->insertItem(0, primary_pixmap_item_);
        main_layout_->insertItem(0, info_layout_);
    }

    main_layout_->setAlignment(info_layout_, Qt::AlignLeft);
    main_layout_->setAlignment(primary_pixmap_item_, Qt::AlignCenter);
    themeChanged();
}

/**
 *
 * @return
 */
QRectF NamePlate::getIconGeometry() const
{
    return primary_pixmap_item_->geometry();
}

/**
 * @brief NamePlate::scaledPixmap
 * @param path
 * @param name
 * @param size
 * @return
 */
QPixmap NamePlate::scaledPixmap(const QString& path, const QString& name, const QSize& size)
{
    auto pixmap = Theme::theme()->getImage(path, name,QSize(), Theme::theme()->getMenuIconColor());
    return pixmap.scaled(size, Qt::KeepAspectRatio, Qt::SmoothTransformation);
}

/**
 * @brief NamePlate::themeChanged
 */
void NamePlate::themeChanged()
{
    primary_text_item_->setDefaultTextColor(Theme::theme()->getTextColor());
    secondary_text_item_->setDefaultTextColor(Theme::theme()->getTextColor());

    primary_pixmap_item_->setPixmap(scaledPixmap(primary_icon_.first, primary_icon_.second, Defaults::primary_icon_size));
    secondary_pixmap_item_->setPixmap(scaledPixmap(secondary_icon_.first, secondary_icon_.second, Defaults::secondary_icon_size));
}

/**
 * @brief NamePlate::setupLayout
 */
void NamePlate::setupLayout()
{
    auto meta_layout = new QGraphicsLinearLayout(Qt::Horizontal);
    meta_layout->setSpacing(Defaults::tertiary_layout_spacing);
    meta_layout->setContentsMargins(Defaults::padding, 0, 0, 0);
    meta_layout->addItem(secondary_pixmap_item_);
    meta_layout->addItem(secondary_text_item_);
    meta_layout->setAlignment(secondary_pixmap_item_, Qt::AlignCenter);
    meta_layout->setStretchFactor(secondary_text_item_, 1);

    info_layout_ = new QGraphicsLinearLayout(Qt::Vertical);
    info_layout_->setSpacing(0);
    info_layout_->setContentsMargins(0, Defaults::layout_margin, 0, Defaults::layout_margin);
    info_layout_->addItem(primary_text_item_);
    info_layout_->addItem(meta_layout);

    main_layout_ = new QGraphicsLinearLayout(Qt::Horizontal, this);
    main_layout_->setSpacing(Defaults::primary_layout_spacing);
    main_layout_->setContentsMargins(Defaults::layout_margin, 0, Defaults::layout_margin, 0);
    main_layout_->addItem(primary_pixmap_item_);
    main_layout_->addItem(info_layout_);
    main_layout_->setAlignment(primary_pixmap_item_, Qt::AlignCenter);
    main_layout_->setStretchFactor(info_layout_, 1);
}

#include <QPainter>
void NamePlate::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    return;
    painter->setBrush(Qt::NoBrush);
    painter->setPen(Qt::red);
    painter->drawRect(primary_pixmap_item_->geometry());
    painter->drawRect(primary_text_item_->geometry());
    painter->setPen(Qt::green);
    painter->drawRect(secondary_pixmap_item_->geometry());
    painter->drawRect(secondary_text_item_->geometry());
    painter->setPen(Qt::magenta);
    painter->drawRect(info_layout_->geometry());
}