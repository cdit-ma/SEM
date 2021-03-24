//
// Created by Cathlyn Aston on 24/9/20.
//

#include "nameplate.h"
#include "../../../theme.h"
#include "../pulseviewutils.h"

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

    primary_pixmap_item_->setMaximumSize(primary_icon_size_);
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
    primary_pixmap_item_->setPixmap(Utils::scaledPixmap(icon_path, icon_name, primary_icon_size_));
    primary_pixmap_item_->updateGeometry();
}

/**
 * @brief NamePlate::changeName
 * @param name
 * @throws std::runtime_error
 */
void NamePlate::changeName(const QString& name)
{
    throw std::runtime_error("Renaming of Pulse entities/containers have not yet been implemented");
    /*
    prepareGeometryChange();
    primary_text_item_->setPlainText(name);
    primary_text_item_->updateGeometry();
    updateGeometry();
     */
}

/**
 * @brief NamePlate::setIconPos
 * @param pos
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
        main_layout_->addItem(primary_pixmap_item_);
        main_layout_->addItem(info_layout_);
    } else {
        main_layout_->addItem(info_layout_);
        main_layout_->addItem(primary_pixmap_item_);
    }

    main_layout_->setAlignment(primary_pixmap_item_, Qt::AlignCenter);
    main_layout_->setAlignment(info_layout_, Qt::AlignLeft);
    main_layout_->setStretchFactor(info_layout_, 1);
    themeChanged();
}

/**
 * @brief NamePlate::getIconGeometry
 * @return
 */
QRectF NamePlate::getIconGeometry() const
{
    return primary_pixmap_item_->geometry();
}

/**
 * @brief NamePlate::setPrimarySpacing
 * @param spacing
 */
void NamePlate::setPrimarySpacing(int spacing)
{
    prepareGeometryChange();
    main_layout_->setSpacing(spacing);
    main_layout_->activate();
    update();
}

/**
 * @brief NamePlate::setPrimaryIconSize
 * @param width
 * @param height
 */
void NamePlate::setPrimaryIconSize(int width, int height)
{
    primary_icon_size_ = QSize(width, height);
    primary_pixmap_item_->setPixmap(Utils::scaledPixmap(primary_icon_.first, primary_icon_.second, primary_icon_size_));
    primary_pixmap_item_->updateGeometry();

    // Re-center the new pixmap in its geometry
    auto pix_geom = getIconGeometry();
    if (!pix_geom.isNull()) {
        auto pix_size = primary_pixmap_item_->pixmap().size();
        auto offset = (pix_geom.size() - pix_size) / 2.0;
        primary_pixmap_item_->setOffset(offset.width(), offset.height());
    }

    main_layout_->activate();
    update();
}

/**
 * @brief NamePlate::themeChanged
 */
void NamePlate::themeChanged()
{
    primary_text_item_->setDefaultTextColor(Theme::theme()->getTextColor());
    secondary_text_item_->setDefaultTextColor(Theme::theme()->getTextColor());

    primary_pixmap_item_->setPixmap(Utils::scaledPixmap(primary_icon_.first, primary_icon_.second, primary_icon_size_));
    secondary_pixmap_item_->setPixmap(Utils::scaledPixmap(secondary_icon_.first, secondary_icon_.second, Defaults::secondary_icon_size));
}

/**
 * @brief NamePlate::setupLayout
 */
void NamePlate::setupLayout()
{
    auto&& margin = Defaults::layout_padding;
    auto meta_layout = new QGraphicsLinearLayout(Qt::Horizontal);
    meta_layout->setSpacing(margin / 2.0);
    meta_layout->setContentsMargins(0, 0, 0, 0);
    meta_layout->addItem(secondary_pixmap_item_);
    meta_layout->addItem(secondary_text_item_);
    meta_layout->setAlignment(secondary_pixmap_item_, Qt::AlignCenter);
    meta_layout->setAlignment(secondary_text_item_, Qt::AlignCenter);
    meta_layout->setStretchFactor(secondary_text_item_, 1);

    info_layout_ = new QGraphicsLinearLayout(Qt::Vertical);
    info_layout_->setContentsMargins(0, 0, 0, 0);
    info_layout_->setSpacing(0);
    info_layout_->addStretch(1);
    info_layout_->addItem(primary_text_item_);
    info_layout_->addItem(meta_layout);
    info_layout_->addStretch(1);

    main_layout_ = new QGraphicsLinearLayout(Qt::Horizontal, this);
    main_layout_->setSpacing(margin);
    main_layout_->setContentsMargins(margin, margin, margin, margin);
    main_layout_->addItem(primary_pixmap_item_);
    main_layout_->addItem(info_layout_);
    main_layout_->setAlignment(primary_pixmap_item_, Qt::AlignCenter);
    main_layout_->setStretchFactor(info_layout_, 1);
}