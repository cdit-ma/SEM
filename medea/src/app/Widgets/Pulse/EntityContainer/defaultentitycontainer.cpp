//
// Created by Cathlyn Aston on 28/10/20.
//

#include "defaultentitycontainer.h"
#include "../pulseviewutils.h"

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
      tray_(new FreeFormTray(this)),
      input_delegate_anchor_(new DelegateAnchor(this)),
      output_delegate_anchor_(new DelegateAnchor(this))
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

    auto update_anchors = [this]() {
        if (isVisible()) {
            getInputAnchor()->triggerPositionChange(topRect().left(), topRect().center().y());
            getOutputAnchor()->triggerPositionChange(topRect().right(), topRect().center().y());
        }
    };
    connect(this, &DefaultEntityContainer::geometryChanged, [=]() { update_anchors(); });
    update_anchors();

    // When the tray's geometry has changed, update the container's geometry and schedule a repaint
    connect(tray_, &QGraphicsWidget::geometryChanged, [this]() {
        updateGeometry();
        update();
    });

    setFlags(flags() | QGraphicsWidget::ItemIsMovable);
    connect(Theme::theme(), &Theme::theme_Changed, [this]() { themeChanged(); });
    themeChanged();
}

/**
 * @brief DefaultEntityContainer::connectModelData
 * @param model_data
 * @throws std::invalid_argument
 */
void DefaultEntityContainer::connectModelData(QPointer<Pulse::Model::Entity> model_data)
{
    if (model_data.isNull()) {
        throw std::invalid_argument("DefaultEntityContainer::connectModelData - The model data is null");
    }
    connect(model_data, &Pulse::Model::Entity::destroyed, this, &DefaultEntityContainer::onModelDeleted);
    connect(model_data, &Pulse::Model::Entity::nameChanged, name_plate_, &NamePlate::changeName);
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
 * @brief DefaultEntityContainer::getInputAnchor
 * @throws std::runtime_error
 * @return
 */
DelegateAnchor* DefaultEntityContainer::getInputAnchor()
{
    if (input_delegate_anchor_ == nullptr) {
        throw std::runtime_error("DefaultEntityContainer::getInputAnchor - The input delegate anchor is null");
    }
    return input_delegate_anchor_;
}

/**
 * @brief DefaultEntityContainer::getOutputAnchor
 * @throws std::runtime_error
 * @return
 */
DelegateAnchor* DefaultEntityContainer::getOutputAnchor()
{
    if (output_delegate_anchor_ == nullptr) {
        throw std::runtime_error("DefaultEntityContainer::getOutputAnchor - The input delegate anchor is null");
    }
    return output_delegate_anchor_;
}

/**
 * @brief DefaultEntityContainer::add
 * @param entity
 */
void DefaultEntityContainer::add(Entity* entity)
{
    auto widget = Utils::getEntityAsGraphicsWidget(entity);
    widget->setParentItem(this);
    connect(this, &DefaultEntityContainer::geometryChanged, widget, &QGraphicsWidget::geometryChanged);

    auto connectable_child = dynamic_cast<Connectable*>(widget);
    if (connectable_child != nullptr) {
        connect(widget, &DefaultEntityContainer::visibleChanged, [this, connectable_child, widget]() {
            childVisibilityChanged(connectable_child, widget->isVisible());
        });
    }

    prepareGeometryChange();
    tray_->addItem(widget);
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
 * @brief DefaultEntityContainer::setPrimaryIconSize
 * @param width
 * @param height
 */
void DefaultEntityContainer::setPrimaryIconSize(int width, int height)
{
    name_plate_->setPrimaryIconSize(width, height);
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
 * @brief DefaultEntityContainer::childVisibilityChanged
 * @param child
 * @param visible
 * @throws std::runtime_error
 */
void DefaultEntityContainer::childVisibilityChanged(Connectable* child, bool visible)
{
    if (child == nullptr) {
        throw std::runtime_error("DefaultEntityContainer::childVisibilityChanged - Child entity container is null");
    }

    auto update_child_anchor = [] (bool child_visible, EdgeAnchor* anchor, EdgeAdopter& adopter) {
        if (anchor != nullptr) {
            if (child_visible) {
                anchor->retrieveFromAdopter();
            } else {
                anchor->transferToAdopter(&adopter);
            }
        }
    };

    update_child_anchor(visible, child->getInputAnchor(), *getInputAnchor());
    update_child_anchor(visible, child->getOutputAnchor(), *getOutputAnchor());
}

/**
 * @brief DefaultEntityContainer::setGeometry
 * @param geom
 */
void DefaultEntityContainer::setGeometry(const QRectF& geom)
{
    // Force this item's geometry to have the same size as the bounding rect
    prepareGeometryChange();
    QGraphicsWidget::setGeometry(QRectF(geom.topLeft(), boundingRect().size()));
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

    int level = Utils::getDepth(this) + 1;
    if (theme->getTextColor() == theme->black()) {
        tray_color_ = tray_color_.lighter(100 + 5 * level);
    } else {
        tray_color_ = tray_color_.lighter(100 + 20 * level);
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