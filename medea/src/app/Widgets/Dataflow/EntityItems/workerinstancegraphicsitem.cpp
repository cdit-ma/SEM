//
// Created by Cathlyn Aston on 17/8/20.
//

#include "workerinstancegraphicsitem.h"
#include "componentinstancegraphicsitem.h"
#include "../../../theme.h"

#include <QPainter>

const int icon_size = 50;
const int pixmap_padding = 15;

WorkerInstanceGraphicsItem::WorkerInstanceGraphicsItem(const WorkerInstanceData& worker_data, ComponentInstanceGraphicsItem* parent)
    : QGraphicsWidget(parent),
      parent_comp_inst_item_(parent),
      worker_inst_data_(worker_data)
{
    setupLayout();
    setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    setFlags(flags() | QGraphicsWidget::ItemIsSelectable | QGraphicsWidget::ItemSendsGeometryChanges | QGraphicsWidget::ItemIsMovable);

    connect(Theme::theme(), &Theme::theme_Changed, this, &WorkerInstanceGraphicsItem::themeChanged);
    themeChanged();
}

const QString& WorkerInstanceGraphicsItem::getGraphmlID() const
{
    return worker_inst_data_.getGraphmlID();
}

const QString& WorkerInstanceGraphicsItem::getName() const
{
    return worker_inst_data_.getName();
}

const QString& WorkerInstanceGraphicsItem::getType() const
{
    return worker_inst_data_.getType();
}

QRectF WorkerInstanceGraphicsItem::boundingRect() const
{
    return QRectF(0, 0, getWidth(), getHeight());
}

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

void WorkerInstanceGraphicsItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    painter->setBrush(active_color_);
    painter->drawRoundedRect(boundingRect(), 2, 2);
}

qreal WorkerInstanceGraphicsItem::getWidth() const
{
    return main_layout_->geometry().width();
}

qreal WorkerInstanceGraphicsItem::getHeight() const
{
    return main_layout_->minimumHeight();
}

void WorkerInstanceGraphicsItem::themeChanged()
{
    Theme* theme = Theme::theme();
    label_text_item_->setDefaultTextColor(theme->getTextColor());

    auto pixmap = theme->getImage("Icons", "spanner", QSize(), theme->getTextColor());
    icon_pixmap_item_->updatePixmap(pixmap);

    highlight_color_ = theme->getHighlightColor();
    //default_color_ = theme->getTextColor(ColorRole::SELECTED);
    default_color_ = theme->getDisabledBackgroundColor();
    active_color_ = default_color_;
}

void WorkerInstanceGraphicsItem::setupLayout()
{
    QPixmap pix = Theme::theme()->getImage("Icons", "spanner");
    icon_pixmap_item_ = new PixmapGraphicsItem(pix, this);
    icon_pixmap_item_->setPixmapPadding(pixmap_padding);
    icon_pixmap_item_->setParentItem(this);
    icon_pixmap_item_->setSquareSize(icon_size);
    icon_pixmap_item_->setMaximumHeight(icon_pixmap_item_->minimumHeight());

    label_text_item_ = new TextGraphicsItem(getName(), this);
    label_text_item_->setParentItem(this);
    label_text_item_->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);
    label_text_item_->setTextAlignment(Qt::AlignVCenter | Qt::AlignLeft);

    main_layout_ = new QGraphicsLinearLayout(Qt::Horizontal);
    main_layout_->setSpacing(3);
    main_layout_->setContentsMargins(0, 0, 10, 0);
    main_layout_->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);

    main_layout_->addItem(icon_pixmap_item_);
    main_layout_->addItem(label_text_item_);

    prepareGeometryChange();
    setContentsMargins(0, 0, 0, 0);
    setLayout(main_layout_);
}