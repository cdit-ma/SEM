//
// Created by Cathlyn Aston on 5/11/20.
//

#include "portinstance.h"
#include "../pulseviewutils.h"

#include <QPainter>
#include <QTimer>

using namespace Pulse::View;
const int flash_duration_ms = Defaults::flash_duration_ms;

/**
 * @brief PortInstance::PortInstance
 * @param port_data
 * @param parent
 * @throws std::invalid_argument
 */
PortInstance::PortInstance(const QPointer<PortInstanceData>& port_data, QGraphicsItem* parent)
    : QGraphicsWidget(parent),
      port_inst_data_(port_data),
      name_plate_(new NamePlate(port_data->getName(), "Icons", Utils::getPortIconName(port_data->getKind()),
                                "Message", "Icons", "envelopeTwoTone",this))
{
    if (port_data.isNull()) {
        throw std::invalid_argument("PortInstanceGraphicsItem::PortInstanceGraphicsItem - Port data is null");
    }

    auto icon_pos = NamePlate::IconPos::Left;
    switch (port_data->getKind()) {
        case AggServerResponse::Port::Kind::PERIODIC:
            default_ellipse_color_ = Defaults::input_port_color;
            break;
        case AggServerResponse::Port::Kind::SUBSCRIBER:
            [[fallthrough]];
        case AggServerResponse::Port::Kind::REPLIER:
            default_ellipse_color_ = Defaults::input_port_color;
            input_anchor_ = new NaturalAnchor(this);
            connect(this, &PortInstance::flashedPortLifecycle, &input_anchor_->getEdgeConnector(), &EdgeConnector::flashPortLifecycle);
            connect(this, &PortInstance::unflashedPortLifecycle, &input_anchor_->getEdgeConnector(), &EdgeConnector::unflashPortLifecycle);
            break;
        case AggServerResponse::Port::Kind::PUBLISHER:
            [[fallthrough]];
        case AggServerResponse::Port::Kind::REQUESTER:
            icon_pos = NamePlate::IconPos::Right;
            default_ellipse_color_ = Defaults::output_port_color;
            output_anchor_ = new NaturalAnchor(this);
            connect(this, &PortInstance::flashedPortLifecycle, &output_anchor_->getEdgeConnector(), &EdgeConnector::flashPortLifecycle);
            connect(this, &PortInstance::unflashedPortLifecycle, &output_anchor_->getEdgeConnector(), &EdgeConnector::unflashPortLifecycle);
            break;
        case AggServerResponse::Port::Kind::NO_KIND:
            throw std::invalid_argument("PortInstanceGraphicsItem::PortInstanceGraphicsItem - Port kind is unknown");
    }

    ellipse_pen_ = QPen(Qt::transparent, 3.0);
    ellipse_pen_color_ = Theme::theme()->getTextColor();
    active_ellipse_color_ = default_ellipse_color_;
    highlight_ellipse_color_ = Theme::theme()->getHighlightColor();

    name_plate_->setIconPos(icon_pos);
    name_plate_->setPrimarySpacing(Defaults::layout_padding);

    auto icon_size = Defaults::primary_icon_size * 0.75;
    name_plate_->setPrimaryIconSize(icon_size.width(), icon_size.height());

    auto layout = new QGraphicsLinearLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addItem(name_plate_);

    const auto& icon_geom = name_plate_->getIconGeometry();
    if (input_anchor_ != nullptr) {
        input_anchor_->setPos(icon_geom.left(), icon_geom.center().y());
    }
    if (output_anchor_ != nullptr) {
        output_anchor_->setPos(icon_geom.right(), icon_geom.center().y());
    }

    auto connect_anchor = [this, icon_pos] (NaturalAnchor* anchor) {
        if (anchor != nullptr) {
            connect(this, &PortInstance::geometryChanged, [this, anchor, icon_pos]() {
                const auto& icon_geom = name_plate_->getIconGeometry();
                auto x = (icon_pos == NamePlate::Left) ? icon_geom.left() : icon_geom.right();
                if (isVisible()) { anchor->triggerPositionChange(x, icon_geom.center().y()); }
            });
        }
    };

    connect_anchor(input_anchor_);
    connect_anchor(output_anchor_);
    connect(Theme::theme(), &Theme::theme_Changed, this, &PortInstance::themeChanged);
}

/**
 * @brief PortInstance::connectModelData
 * @param model_data
 * @throws std::invalid_argument
 */
void PortInstance::connectModelData(QPointer<Pulse::Model::Entity> model_data)
{
    if (model_data.isNull()) {
        throw std::invalid_argument("PortInstance::connectModelData - The model data is null");
    }
    connect(model_data, &Pulse::Model::Entity::destroyed, this, &PortInstance::onModelDeleted);
    connect(model_data, &Pulse::Model::Entity::nameChanged, name_plate_, &NamePlate::changeName);
}

/**
 * @brief PortInstance::onModelDeleted
 */
void PortInstance::onModelDeleted()
{
    deleteLater();
}

/**
 * @brief PortInstance::getInputAnchor
 * @return
 */
NaturalAnchor* PortInstance::getInputAnchor()
{
    return input_anchor_;
}

/**
 * @brief PortInstance::getOutputAnchor
 * @return
 */
NaturalAnchor* PortInstance::getOutputAnchor()
{
    return output_anchor_;
}

/**
 * @brief PortInstance::getAsGraphicsWidget
 * @return
 */
QGraphicsWidget* PortInstance::getAsGraphicsWidget()
{
    return this;
}

/**
 * @brief PortInstance::getKind
 * @return
 */
AggServerResponse::Port::Kind PortInstance::getKind() const
{
    return port_inst_data_->getKind();
}

/**
 * @brief PortInstance::getPreviousEventTime
 * @param time
 * @return
 */
qint64 PortInstance::getPreviousEventTime(qint64 time) const
{
    return port_inst_data_->getPreviousEventTime(time);
}

/**
 * @brief PortInstance::getNextEventTime
 * @param time
 * @return
 */
qint64 PortInstance::getNextEventTime(qint64 time) const
{
    return port_inst_data_->getNextEventTime(time);
}

/**
 * @brief PortInstance::flashEvents
 * @param from_time
 * @param to_time
 */
void PortInstance::flashEvents(qint64 from_time, qint64 to_time)
{
    const auto& port_lifecycle_event_series = port_inst_data_->getPortLifecycleSeries();
    const auto& port_lifecycle_events = port_lifecycle_event_series->getEventsBetween(from_time, to_time);
    if (!port_lifecycle_events.isEmpty()) {
        flashPort(MEDEA::ChartDataKind::PORT_LIFECYCLE, from_time);
    }

    const auto& port_event_series = port_inst_data_->getPortEventSeries();
    const auto& port_events = port_event_series->getEventsBetween(from_time, to_time);

    bool has_port_events = false;
    for (const auto& event : port_events) {
        auto port_event = qobject_cast<PortEvent*>(event);
        if (port_event->getType() != PortEvent::PortEventType::FINISHED_FUNC) {
            has_port_events = true;
            break;
        }
    }
    if (has_port_events) {
        flashPort(MEDEA::ChartDataKind::PORT_EVENT, from_time);
        if (isEventSourcePort()) {
            emit flashEdge(from_time, Pulse::View::Defaults::flash_duration_ms);
        }
    }
}

/**
 * @brief PortInstance::boundingRect
 * @return
 */
QRectF PortInstance::boundingRect() const
{
    return name_plate_->boundingRect();
}

/**
 * @brief PortInstance::paint
 * @param painter
 * @param option
 * @param widget
 */
void PortInstance::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    painter->setRenderHint(QPainter::Antialiasing, true);
    painter->setPen(ellipse_pen_);
    painter->setBrush(active_ellipse_color_);

    auto ellipse_geom = name_plate_->getIconGeometry();
    painter->drawEllipse(ellipse_geom.center(), ellipse_geom.height() / 2.0, ellipse_geom.height() / 2.0);
}

/**
 * @brief PortInstance::themeChanged
 */
void PortInstance::themeChanged()
{
    Theme* theme = Theme::theme();

    bool update_highlight_color = active_ellipse_color_ == highlight_ellipse_color_;
    highlight_ellipse_color_ = theme->getHighlightColor();
    if (update_highlight_color) {
        active_ellipse_color_ = highlight_ellipse_color_;
    }

    bool update_pen_color = ellipse_pen_.color() == ellipse_pen_color_;
    ellipse_pen_color_ = theme->getTextColor();
    if (update_pen_color) {
        ellipse_pen_ = QPen(ellipse_pen_color_, 3.0);
    }

    update();
}

/**
 * @brief PortInstance::flashPort
 * @param event_kind
 * @param from_time
 */
void PortInstance::flashPort(MEDEA::ChartDataKind event_kind, qint64 from_time)
{
    // This function can be called multiple times by multiple callers
    // Due to this, the flash end time needs to be stored and updated to avoid the flash being stopped prematurely
    auto&& end_time = from_time + flash_duration_ms;

    // Switch the color
    if (event_kind == MEDEA::ChartDataKind::PORT_LIFECYCLE) {
        ellipse_pen_.setColor(ellipse_pen_color_);
        port_lifecycle_flash_end_time_ = qMax(end_time, port_lifecycle_flash_end_time_);
        emit flashedPortLifecycle();
    } else if (event_kind == MEDEA::ChartDataKind::PORT_EVENT) {
        active_ellipse_color_ = highlight_ellipse_color_;
        port_event_flash_end_time_ = qMax(end_time, port_event_flash_end_time_);
        emit flashedPortEvent();
    }
    update();

    QTimer::singleShot(flash_duration_ms, this, [this, event_kind, end_time]() {
        unflashPort(event_kind, end_time);
    });
}

/**
 * @abrief PortInstance::unflashPort
 * @param event_kind
 * @param flash_end_time
 */
void PortInstance::unflashPort(MEDEA::ChartDataKind event_kind, qint64 flash_end_time)
{
    // Reset the color when we've reached the flash end time
    if (event_kind == MEDEA::ChartDataKind::PORT_LIFECYCLE) {
        if (port_lifecycle_flash_end_time_ == flash_end_time) {
            ellipse_pen_.setColor(Qt::transparent);
            port_lifecycle_flash_end_time_ = 0;
            emit unflashedPortLifecycle();
        }
    } else if (event_kind == MEDEA::ChartDataKind::PORT_EVENT) {
        if (port_event_flash_end_time_ == flash_end_time) {
            active_ellipse_color_ = default_ellipse_color_;
            port_event_flash_end_time_ = 0;
            emit unflashedPortEvent();
        }
    }
    update();
}

/**
 * @brief PortInstance::isEventSourcePort()
 * @return
 */
bool PortInstance::isEventSourcePort() const
{
    using kind = AggServerResponse::Port::Kind;
    return (getKind() == kind::PUBLISHER ||
            getKind() == kind::REQUESTER ||
            getKind() == kind::REPLIER);
}