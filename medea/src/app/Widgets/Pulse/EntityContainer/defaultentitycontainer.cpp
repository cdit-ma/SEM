//
// Created by Cathlyn Aston on 28/10/20.
//

#include "defaultentitycontainer.h"
#include "../../../theme.h"

#include <QPainter>
#include <QGraphicsSceneMouseEvent>

using namespace Pulse::View;

const qreal pen_width = Defaults::pen_width;
const int tray_padding = Defaults::layout_padding;

/**
 * @brief DefaultEntityContainer::DefaultEntityContainer
 * @param label
 * @param icon_path
 * @param icon_name
 * @param meta_label
 * @param meta_icon_path
 * @param meta_icon_name
 * @param parent
 */
DefaultEntityContainer::DefaultEntityContainer(const QString& label,
                                               const QString& icon_path,
                                               const QString& icon_name,
                                               const QString& meta_label,
                                               const QString& meta_icon_path,
                                               const QString& meta_icon_name,
                                               QGraphicsItem* parent)
    : QGraphicsWidget(parent),
      name_plate_(new NamePlate(label, icon_path, icon_name, meta_label, meta_icon_path, meta_icon_name, this)),
      tray_(new FreeFormTray(this))
{
    auto tray_layout = new QGraphicsLinearLayout();
    tray_layout->setContentsMargins(tray_padding, tray_padding, tray_padding, tray_padding);
    tray_layout->addItem(tray_);

    auto main_layout_ = new QGraphicsLinearLayout(Qt::Vertical, this);
    main_layout_->setSpacing(0);
    main_layout_->setContentsMargins(0, 0, 0, 0);
    main_layout_->addItem(name_plate_);
    main_layout_->addItem(tray_layout);
    main_layout_->setStretchFactor(tray_layout, 1);

    setFlags(flags() | QGraphicsWidget::ItemIsMovable);

    // Connect the theme to update the colors
    connect(Theme::theme(), &Theme::theme_Changed, [this]() { themeChanged(); });
    themeChanged();

    // When the tray's geometry has changed, update the container's geometry and schedule a repaint
    connect(tray_, &QGraphicsWidget::geometryChanged, [this]() {
        updateGeometry();
        update();
    });
}

/**
 * @brief DefaultEntityContainer::connectModelData
 * @param model_data
 */
void DefaultEntityContainer::connectModelData(QPointer<Pulse::Model::Entity> model_data)
{
    if (model_data.isNull()) {
        throw std::invalid_argument("DefaultEntityContainer - The model data is null");
    }
    connect(model_data, &Pulse::Model::Entity::destroyed, this, &DefaultEntityContainer::onModelDeleted);
    connect(model_data, &Pulse::Model::Entity::labelChanged, name_plate_, &NamePlate::changeLabel);
    connect(model_data, &Pulse::Model::Entity::iconChanged, name_plate_, &NamePlate::changeIcon);
}

/**
 * @brief DefaultEntityContainer::onModelDeleted
 */
void DefaultEntityContainer::onModelDeleted()
{
    deleteLater();
}

/**
 * @brief DefaultEntityContainer::getAsGraphicsWidget
 * @return
 */
QGraphicsWidget* DefaultEntityContainer::getAsGraphicsWidget()
{
    return this;
}

/**
 * @brief DefaultEntityContainer::add
 * @param entity
 */
void DefaultEntityContainer::add(Entity* entity)
{
    prepareGeometryChange();
    tray_->addItem(getEntityGraphicsWidget(entity));
}

/**
 * @brief DefaultEntityContainer::expand
 */
void DefaultEntityContainer::expand()
{
    prepareGeometryChange();
    tray_->setVisible(true);
    update();
}

/**
 * @brief DefaultEntityContainer::contract
 */
void DefaultEntityContainer::contract()
{
    prepareGeometryChange();
    tray_->setVisible(false);
    update();
}

/**
 * @brief DefaultEntityContainer::paint
 * @param painter
 * @param option
 * @param widget
 */
void DefaultEntityContainer::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    qreal half_pen = border_pen_.widthF() / 2.0;
    auto rect = boundingRect().adjusted(half_pen, half_pen, -half_pen, -half_pen);

    painter->fillRect(rect, tray_color_);
    painter->fillRect(topRect().adjusted(half_pen, half_pen, -half_pen, -half_pen), top_color_);

    painter->setPen(border_pen_);
    painter->drawRoundedRect(rect, 2, 2);
}

/**
 * @brief DefaultEntityContainer::boundingRect
 * @return
 */
QRectF DefaultEntityContainer::boundingRect() const
{
    return {0, 0, getWidth(), getHeight()};
}

/**
 * @brief DefaultEntityContainer::setPrimaryIconSize
 * @param width
 * @param height
 */
void DefaultEntityContainer::setPrimaryIconSize(int width, int height)
{
    name_plate_->setPrimaryIconSize(width, height);
}

/**
 * @brief DefaultEntityContainer::setGeometry
 * @param geom
 */
void DefaultEntityContainer::setGeometry(const QRectF& geom)
{
    // Force this item's geometry to have the same size as the bounding rect
    prepareGeometryChange();
    QRectF adjusted_rect(geom.topLeft(), boundingRect().size());
    QGraphicsWidget::setGeometry(adjusted_rect);
}

/**
 * @brief DefaultEntityContainer::itemChange
 * @param change
 * @param value
 * @return
 */
QVariant DefaultEntityContainer::itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant& value)
{
    if (change == ItemPositionChange) {
        if (mouse_pressed_) {
            if (top_rect_pressed_) {
                if (parentItem()) {
                    // If the item has a parent, don't allow its position to go past the parent tray's padding
                    auto new_pos = value.toPoint();
                    new_pos.setX(qMax(0, new_pos.x()));
                    new_pos.setY(qMax(0, new_pos.y()));
                    return new_pos;
                }
                // If the item is a top level item, allow it to move to any position
                return value;
            }
            // If the item is being moved by a click outside of the top rect followed by a drag, do nothing
            return pos();
        }
    }
    return QGraphicsWidget::itemChange(change, value);
}

/**
 * @brief DefaultEntityContainer::themeChanged
 */
void DefaultEntityContainer::themeChanged()
{
    Theme* theme = Theme::theme();
    top_color_ = theme->getActiveWidgetBorderColor();
    border_pen_ = QPen(top_color_, Defaults::pen_width);
    tray_color_ = theme->getBackgroundColor();

    int level = 1;
    auto parent_item = parentItem();
    while (parent_item != nullptr) {
        level++;
        parent_item = parent_item->parentItem();
    }

    if (theme->getTextColor() == theme->black()) {
        tray_color_ = tray_color_.lighter(100 + 5 * level);
    } else {
        tray_color_ = tray_color_.lighter(100 + 15 * level);
    }

    update();
}

/**
 * @brief DefaultEntityContainer::mouseDoubleClickEvent
 * @param event
 */
void DefaultEntityContainer::mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event)
{
    if (name_plate_->getIconGeometry().contains(event->pos())) {
        if (inContractedState()) {
            expand();
        } else {
            contract();
        }
    }
    QGraphicsWidget::mouseDoubleClickEvent(event);
}

/**
 * @brief DefaultEntityContainer::mousePressEvent
 * @param event
 */
void DefaultEntityContainer::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    if (topRect().contains(event->pos())) {
        top_rect_pressed_ = true;
    }
    mouse_pressed_ = true;
    QGraphicsWidget::mousePressEvent(event);
}

/**
 * @brief DefaultEntityContainer::mouseReleaseEvent
 * @param event
 */
void DefaultEntityContainer::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    mouse_pressed_ = false;
    top_rect_pressed_ = false;
    QGraphicsWidget::mouseReleaseEvent(event);
}

/**
 * @brief DefaultEntityContainer::topRect
 * @return
 */
QRectF DefaultEntityContainer::topRect() const
{
    if (inContractedState()) {
        return {0, 0, getWidth(), contractedRect().height()};
    } else {
        return {0, 0, getWidth(), contractedRect().height() + pen_width};
    }
}

/**
 * @brief DefaultEntityContainer::contractedRect
 * @return
 */
QRectF DefaultEntityContainer::contractedRect() const
{
    return {0, 0, name_plate_->minimumWidth(), name_plate_->minimumHeight()};
}

/**
 * @brief DefaultEntityContainer::inContractedRect
 * @return
 */
bool DefaultEntityContainer::inContractedState() const
{
    return tray_->isEmpty() || !tray_->isVisible();
}

/**
 * @brief DefaultEntityContainer::getWidth
 * @return
 */
qreal DefaultEntityContainer::getWidth() const
{
    if (inContractedState()) {
        return contractedRect().width();
    } else {
        auto&& tray_width = tray_->geometry().width() + tray_padding * 2;
        return qMax(contractedRect().width(), tray_width);
    }
}

/**
 * @brief DefaultEntityContainer::getHeight
 * @return
 */
qreal DefaultEntityContainer::getHeight() const
{
    if (inContractedState()) {
        return contractedRect().height();
    } else {
        auto&& tray_height = tray_->geometry().height() + tray_padding * 2;
        return contractedRect().height() + tray_height;
    }
}

/**
 * @brief DefaultEntityContainer::getEntityGraphicsWidget
 * @param entity
 * @throws std::invalid_argument
 * @throws std::runtime_error
 * @return
 */
QGraphicsWidget* DefaultEntityContainer::getEntityGraphicsWidget(Entity* entity)
{
    if (entity == nullptr) {
        throw std::invalid_argument("DefaultEntityContainer - The entity is null");
    }
    auto widget = entity->getAsGraphicsWidget();
    if (widget == nullptr) {
        throw std::runtime_error("DefaultEntityContainer - The entity is not a QGraphicsWidget");
    }
    return widget;
}