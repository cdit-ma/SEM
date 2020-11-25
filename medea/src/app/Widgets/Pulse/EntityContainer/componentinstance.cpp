//
// Created by Cathlyn Aston on 5/11/20.
//

#include "componentinstance.h"
#include "../pulseviewutils.h"

#include <QPainter>
#include <QGraphicsSceneMouseEvent>

using namespace Pulse::View;

const qreal pen_width = Defaults::pen_width;

/**
 * @brief ComponentInstance::ComponentInstance
 * @param label
 * @param meta_label
 * @param parent
 */
ComponentInstance::ComponentInstance(const QString& label,
                                     const QString& meta_label,
                                     QGraphicsItem* parent)
    : QGraphicsWidget(parent),
      name_plate_(new NamePlate(label, "EntityIcons", "ComponentInstance",
                                meta_label, "Icons", "code", this)),
      tray_(new TriColumnTray(this)),
      input_delegate_anchor_(new DelegateAnchor(this)),
      output_delegate_anchor_(new DelegateAnchor(this))
{
    auto icon_size = Defaults::primary_icon_size * 1.15;
    name_plate_->setPrimarySpacing(Defaults::layout_padding);
    name_plate_->setPrimaryIconSize(icon_size.width(), icon_size.height());

    auto main_layout_ = new QGraphicsLinearLayout(Qt::Vertical, this);
    main_layout_->setSpacing(0);
    main_layout_->setContentsMargins(0, 0, 0, 0);
    main_layout_->addItem(name_plate_);
    main_layout_->addItem(tray_);
    main_layout_->setStretchFactor(tray_, 1);

    auto update_anchors = [this]() {
        if (isVisible()) {
            getInputAnchor()->triggerPositionChange(topRect().left(), topRect().center().y());
            getOutputAnchor()->triggerPositionChange(topRect().right(), topRect().center().y());
        }
    };
    connect(this, &ComponentInstance::geometryChanged, [=]() { update_anchors(); });
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
 * @brief ComponentInstance::connectModelData
 * @param model_data
 * @throws std::invalid_argument
 */
void ComponentInstance::connectModelData(QPointer<Pulse::Model::Entity> model_data)
{
    if (model_data.isNull()) {
        throw std::invalid_argument("ComponentInstance::connectModelData - The model data is null");
    }
    connect(model_data, &Pulse::Model::Entity::destroyed, this, &ComponentInstance::onModelDeleted);
    connect(model_data, &Pulse::Model::Entity::nameChanged, name_plate_, &NamePlate::changeName);
    connect(model_data, &Pulse::Model::Entity::iconChanged, name_plate_, &NamePlate::changeIcon);
}

/**
 * @brief ComponentInstance::onModelDeleted
 */
void ComponentInstance::onModelDeleted()
{
    deleteLater();
}

/**
 * @brief ComponentInstance::getAsGraphicsWidget
 * @return
 */
QGraphicsWidget* ComponentInstance::getAsGraphicsWidget()
{
    return this;
}

/**
 * @brief ComponentInstance::getInputAnchor
 * @throws std::runtime_error
 * @return
 */
DelegateAnchor* ComponentInstance::getInputAnchor()
{
    if (input_delegate_anchor_ == nullptr) {
        throw std::runtime_error("ComponentInstance::getInputAnchor - The input delegate anchor is null");
    }
    return input_delegate_anchor_;
}

/**
 * @brief ComponentInstance::getOutputAnchor
 * @throws std::runtime_error
 * @return
 */
DelegateAnchor* ComponentInstance::getOutputAnchor()
{
    if (output_delegate_anchor_ == nullptr) {
        throw std::runtime_error("ComponentInstance::getOutputAnchor - The output delegate anchor is null");
    }
    return output_delegate_anchor_;
}

/**
 * @brief ComponentInstance::add
 * @param entity
 */
void ComponentInstance::add(Entity* entity)
{
    auto widget = Utils::getEntityAsGraphicsWidget(entity);
    widget->setParentItem(this);
    widget->setFlags(flags() ^ QGraphicsWidget::ItemIsMovable);

    connect(this, &ComponentInstance::geometryChanged, widget, &QGraphicsWidget::geometryChanged);
    prepareGeometryChange();

    auto port = dynamic_cast<PortInstance*>(widget);
    if (port != nullptr) {

        connect(port, &PortInstance::visibleChanged, [this, port]() {
            portVisibilityChanged(port);
        });

        switch (port->getKind()) {
            case AggServerResponse::Port::Kind::PERIODIC:
                [[fallthrough]];
            case AggServerResponse::Port::Kind::SUBSCRIBER:
                [[fallthrough]];
            case AggServerResponse::Port::Kind::REPLIER:
                tray_->addLeft(widget);
                return;
            case AggServerResponse::Port::Kind::REQUESTER:
                [[fallthrough]];
            case AggServerResponse::Port::Kind::PUBLISHER:
                tray_->addRight(widget);
                return;
            default:
                break;
        }
    }

    tray_->addCenter(widget);
}

/**
 * @brief ComponentInstance::expand
 */
void ComponentInstance::expand()
{
    prepareGeometryChange();
    tray_->setVisible(true);
    update();
}

/**
 * @brief ComponentInstance::contract
 */
void ComponentInstance::contract()
{
    prepareGeometryChange();
    tray_->setVisible(false);
    update();
}

/**
 * @brief ComponentInstance::paint
 * @param painter
 * @param option
 * @param widget
 */
void ComponentInstance::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
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
 * @brief ComponentInstance::boundingRect
 * @return
 */
QRectF ComponentInstance::boundingRect() const
{
    return {0, 0, getWidth(), getHeight()};
}

/**
 * @brief ComponentInstance::topRect
 * @return
 */
QRectF ComponentInstance::topRect() const
{
    if (inContractedState()) {
        return {0, 0, getWidth(), contractedRect().height()};
    } else {
        return {0, 0, getWidth(), contractedRect().height() + pen_width};
    }
}

/**
 * @brief ComponentInstance::contractedRect
 * @return
 */
QRectF ComponentInstance::contractedRect() const
{
    return {0, 0, name_plate_->minimumWidth(), name_plate_->minimumHeight()};
}

/**
 * @brief ComponentInstance::inContractedState
 * @return
 */
bool ComponentInstance::inContractedState() const
{
    return tray_->isEmpty() || !tray_->isVisible();
}

/**
 * @brief ComponentInstance::getWidth
 * @return
 */
qreal ComponentInstance::getWidth() const
{
    if (inContractedState()) {
        return contractedRect().width();
    } else {
        return qMax(tray_->geometry().width(), contractedRect().width());
    }
}

/**
 * @brief ComponentInstance::getHeight
 * @return
 */
qreal ComponentInstance::getHeight() const
{
    if (inContractedState()) {
        return contractedRect().height();
    } else {
        return contractedRect().height() + tray_->geometry().height();
    }
}

/**
 * @brief ComponentInstance::setGeometry
 * @param rect
 */
void ComponentInstance::setGeometry(const QRectF& rect)
{
    // Force this item's geometry to have the same size as the bounding rect
    prepareGeometryChange();
    QGraphicsWidget::setGeometry(QRectF(rect.topLeft(), boundingRect().size()));
}

/**
 * @brief ComponentInstance::portVisibilityChanged
 * @param port_inst
 * @throws std::runtime_error
 */
void ComponentInstance::portVisibilityChanged(PortInstance* port_inst)
{
    if (port_inst == nullptr) {
        throw std::runtime_error("ComponentInstance::portVisibilityChanged - Port instance is null");
    }

    auto update_port_anchor = [] (bool port_visible, EdgeAnchor* anchor, EdgeAdopter& adopter) {
        if (anchor != nullptr) {
            if (port_visible) {
                anchor->retrieveFromAdopter();
            } else {
                anchor->transferToAdopter(&adopter);
            }
        }
    };

    const bool visible = port_inst->isVisible();
    update_port_anchor(visible, port_inst->getInputAnchor(), *getInputAnchor());
    update_port_anchor(visible, port_inst->getOutputAnchor(), *getOutputAnchor());
}

/**
 * @brief ComponentInstance::itemChange
 * @param change
 * @param value
 * @return
 */
QVariant ComponentInstance::itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant& value)
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
                // If the item is a top level item, allow it to have any position
                return value;
            }
            // If the item is being moved by a click outside of the top rect and then dragged, do nothing
            return pos();
        }
    }
    return QGraphicsWidget::itemChange(change, value);
}

/**
 * @brief ComponentInstance::themeChanged
 */
void ComponentInstance::themeChanged()
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
 * @brief ComponentInstance::mouseDoubleClickEvent
 * @param event
 */
void ComponentInstance::mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event)
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
 * @brief ComponentInstance::mousePressEvent
 * @param event
 */
void ComponentInstance::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    if (topRect().contains(event->pos())) {
        top_rect_pressed_ = true;
    }
    mouse_pressed_ = true;
    QGraphicsWidget::mousePressEvent(event);
}

/**
 * @brief ComponentInstance::mouseReleaseEvent
 * @param event
 */
void ComponentInstance::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    mouse_pressed_ = false;
    top_rect_pressed_ = false;
    QGraphicsWidget::mouseReleaseEvent(event);
}