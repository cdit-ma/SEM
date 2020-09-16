//
// Created by Cathlyn Aston on 17/8/20.
//

#include "workerinstancegraphicsitem.h"
#include "componentinstancegraphicsitem.h"
#include "../../../theme.h"

#include <QPainter>

const int icon_size = 50;
const int pixmap_padding = 15;

/**
 * @brief WorkerInstanceGraphicsItem::WorkerInstanceGraphicsItem
 * @param worker_data
 * @param parent
 */
WorkerInstanceGraphicsItem::WorkerInstanceGraphicsItem(const WorkerInstanceData& worker_data, ComponentInstanceGraphicsItem* parent)
    : QGraphicsWidget(parent),
      worker_inst_data_(worker_data)
{
    setupLayout();
    setFlags(flags() | QGraphicsWidget::ItemIsSelectable | QGraphicsWidget::ItemSendsGeometryChanges | QGraphicsWidget::ItemIsMovable);

    connect(Theme::theme(), &Theme::theme_Changed, this, &WorkerInstanceGraphicsItem::themeChanged);
    themeChanged();
}

/**
 * @brief WorkerInstanceGraphicsItem::getGraphmlID
 * @return
 */
const QString& WorkerInstanceGraphicsItem::getGraphmlID() const
{
    return worker_inst_data_.getGraphmlID();
}

/**
 * @brief WorkerInstanceGraphicsItem::getName
 * @return
 */
const QString& WorkerInstanceGraphicsItem::getName() const
{
    return worker_inst_data_.getName();
}

/**
 * @brief WorkerInstanceGraphicsItem::getType
 * @return
 */
const QString& WorkerInstanceGraphicsItem::getType() const
{
    return worker_inst_data_.getType();
}

/**
 * @brief WorkerInstanceGraphicsItem::boundingRect
 * @return
 */
QRectF WorkerInstanceGraphicsItem::boundingRect() const
{
    return QRectF(0, 0, getWidth(), getHeight());
}

/**
 * @brief WorkerInstanceGraphicsItem::sizeHint
 * @param which
 * @param constraint
 * @return
 */
QSizeF WorkerInstanceGraphicsItem::sizeHint(Qt::SizeHint which, const QSizeF &constraint) const
{
    switch (which) {
        case Qt::MinimumSize:
        case Qt::PreferredSize:
            return main_layout_->minimumSize();
        case Qt::MaximumSize:
            return QSizeF(10000, 10000);
        default:
            break;
    }
    return constraint;
}

/**
 * @brief WorkerInstanceGraphicsItem::paint
 * @param painter
 * @param option
 * @param widget
 */
void WorkerInstanceGraphicsItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    painter->setPen(Qt::NoPen);
    painter->setBrush(active_color_);
    painter->drawRoundedRect(boundingRect(), 2, 2);
}

/**
 * @brief WorkerInstanceGraphicsItem::getWidth
 * @return
 */
qreal WorkerInstanceGraphicsItem::getWidth() const
{
    return main_layout_->geometry().width();
}

/**
 * @brief WorkerInstanceGraphicsItem::getHeight
 * @return
 */
qreal WorkerInstanceGraphicsItem::getHeight() const
{
    return main_layout_->geometry().height();
}

/**
 * @brief WorkerInstanceGraphicsItem::themeChanged
 */
void WorkerInstanceGraphicsItem::themeChanged()
{
    Theme* theme = Theme::theme();
    highlight_color_ = theme->getHighlightColor();
    default_color_ = theme->getAltBackgroundColor();
    active_color_ = default_color_;

    auto pixmap = theme->getImage("Icons", "spanner");
    icon_pixmap_item_->updatePixmap(pixmap);
    label_text_item_->setDefaultTextColor(theme->getTextColor());

    pixmap = Theme::theme()->getImage("Icons", "code");
    metadata_pixmap_item_->updatePixmap(pixmap);
    metadata_text_item_->setDefaultTextColor(theme->getTextColor());
}

/**
 * @brief WorkerInstanceGraphicsItem::setupLayout
 */
void WorkerInstanceGraphicsItem::setupLayout()
{
    icon_pixmap_item_ = new PixmapGraphicsItem(QPixmap(), this);
    icon_pixmap_item_->setPixmapSquareSize(icon_size);
    icon_pixmap_item_->setPixmapPadding(pixmap_padding);
    icon_pixmap_item_->setMaximumSize(icon_size, icon_size);

    label_text_item_ = new TextGraphicsItem(getName(), this);
    label_text_item_->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);
    label_text_item_->setTextAlignment(Qt::AlignBottom);

    metadata_text_item_ = new TextGraphicsItem("WorkerDefinitionName", this);
    metadata_text_item_->setFont(QFont("Verdana", 8));
    metadata_text_item_->setTextAlignment(Qt::AlignTop);

    int sub_size = icon_size / 2.5;
    metadata_pixmap_item_ = new PixmapGraphicsItem(QPixmap(), this);
    metadata_pixmap_item_->setPixmapSquareSize(sub_size);
    metadata_pixmap_item_->setMaximumHeight(metadata_text_item_->effectiveSizeHint(Qt::PreferredSize).height());

    int info_spacing = 2;
    sub_info_layout_ = new QGraphicsLinearLayout(Qt::Horizontal);
    sub_info_layout_->setSpacing(info_spacing);
    sub_info_layout_->setContentsMargins(info_spacing, 0, 0, 0);
    sub_info_layout_->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);
    sub_info_layout_->addItem(metadata_pixmap_item_);
    sub_info_layout_->setAlignment(metadata_pixmap_item_, Qt::AlignHCenter | Qt::AlignTop);
    sub_info_layout_->setStretchFactor(metadata_pixmap_item_, 0);
    sub_info_layout_->addItem(metadata_text_item_);
    sub_info_layout_->setAlignment(metadata_text_item_, Qt::AlignLeft);
    sub_info_layout_->setStretchFactor(metadata_text_item_, 1);

    info_layout_ = new QGraphicsLinearLayout(Qt::Vertical);
    info_layout_->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);
    info_layout_->setSpacing(info_spacing);
    info_layout_->setContentsMargins(0, 0, 0, info_spacing);
    info_layout_->addItem(label_text_item_);
    info_layout_->addItem(sub_info_layout_);

    main_layout_ = new QGraphicsLinearLayout(Qt::Horizontal);
    main_layout_->setSpacing(0);
    main_layout_->setContentsMargins(0, 0, 5, 0);
    main_layout_->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);
    main_layout_->addItem(icon_pixmap_item_);
    main_layout_->addItem(info_layout_);

    setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);
    setContentsMargins(0, 0, 0, 0);
    setLayout(main_layout_);
}