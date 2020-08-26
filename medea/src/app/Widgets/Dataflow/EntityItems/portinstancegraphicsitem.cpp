#include "portinstancegraphicsitem.h"
#include "componentinstancegraphicsitem.h"
#include "../../../theme.h"

#include <QPainter>
#include <QTimer>

const int icon_size = 50;
const int pixmap_padding = 10;
const int flash_duration_ms = 200;

/**
 * @brief PortInstanceGraphicsItem::PortInstanceGraphicsItem
 * @param port_data
 * @param parent
 * @throws std::invalid_argument
 */
PortInstanceGraphicsItem::PortInstanceGraphicsItem(const PortInstanceData& port_data, ComponentInstanceGraphicsItem* parent)
    : QGraphicsWidget(parent),
      port_inst_data_(port_data)
{
    icon_path.first = "Icons";

    // Identify Port kind
    switch (port_data.getKind()) {
        case AggServerResponse::Port::Kind::PERIODIC:
            icon_path.second = "clockCycle";
            break;
        case AggServerResponse::Port::Kind::PUBLISHER:
            icon_path.second = "arrowRightLong";
            event_src_port_ = true;
            alignment_ = Qt::AlignRight;
            break;
        case AggServerResponse::Port::Kind::SUBSCRIBER:
            icon_path.second = "arrowIntoBox";
            break;
        case AggServerResponse::Port::Kind::REQUESTER:
            icon_path.second = "arrowTopRight";
            event_src_port_ = true;
            alignment_ = Qt::AlignRight;
            break;
        case AggServerResponse::Port::Kind::REPLIER:
            icon_path.second = "arrowBottomRight";
            event_src_port_ = true;
            break;
        case AggServerResponse::Port::Kind::NO_KIND:
            throw std::invalid_argument("PortInstanceGraphicsItem::PortInstanceGraphicsItem - Port kind is unknown");
    }

    setupLayout();
    setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    setFlags(flags() | QGraphicsWidget::ItemIsSelectable | QGraphicsWidget::ItemSendsGeometryChanges);

    connect(this, &PortInstanceGraphicsItem::geometryChanged, this, &PortInstanceGraphicsItem::updateConnectionPos);
    connect(Theme::theme(), &Theme::theme_Changed, this, &PortInstanceGraphicsItem::themeChanged);
    themeChanged();
}

/**
 * @brief PortInstanceGraphicsItem::getGraphmlID
 * @return
 */
const QString& PortInstanceGraphicsItem::getGraphmlID() const
{
    return port_inst_data_.getGraphmlID();
}

/**
 * @brief PortInstanceGraphicsItem::getPortName
 * @return
 */
const QString& PortInstanceGraphicsItem::getPortName() const
{
    return port_inst_data_.getName();
}

/**
 * @brief PortInstanceGraphicsItem::getPortKind
 * @return
 */
AggServerResponse::Port::Kind PortInstanceGraphicsItem::getPortKind() const
{
    return port_inst_data_.getKind();
}

/**
 * @brief PortInstanceGraphicsItem::getEdgePoint
 * This returns the point from which the edge will come out of or in to
 * @return
 */
QPointF PortInstanceGraphicsItem::getEdgePoint() const
{
    bool left_point = true;

    switch (port_inst_data_.getKind()) {
        case AggServerResponse::Port::Kind::SUBSCRIBER:
        case AggServerResponse::Port::Kind::REPLIER:
            break;
        case AggServerResponse::Port::Kind::PUBLISHER:
        case AggServerResponse::Port::Kind::REQUESTER:
            left_point = false;
            break;
        default:
            return QPoint(0,0);
    }

    QRectF scene_rect;
    if (isVisible()) {
        scene_rect = icon_pixmap_item_->sceneBoundingRect();
    } else {
        // Get the first visible parent item and use its scene bounding rect for the edge point
        auto parent_item = parentItem();
        while (parent_item) {
            if (parent_item->isVisible()) {
                scene_rect = parent_item->sceneBoundingRect();
                break;
            }
            parent_item = parent_item->parentItem();
        }
    }

    qreal x = scene_rect.right();
    if (left_point) {
        x = scene_rect.left();
    }

    return QPointF(x, scene_rect.center().y());
}

/**
 * @brief PortInstanceGraphicsItem::getPreviousEventTime
 * @param time
 * @return
 */
qint64 PortInstanceGraphicsItem::getPreviousEventTime(qint64 time) const
{
    return port_inst_data_.getPreviousEventTime(time);
}

/**
 * @brief PortInstanceGraphicsItem::getNextEventTime
 * @param time
 * @return
 */
qint64 PortInstanceGraphicsItem::getNextEventTime(qint64 time) const
{
    return port_inst_data_.getNextEventTime(time);
}

/**
 * @brief PortInstanceGraphicsItem::setAlignment
 * @param alignment
 */
void PortInstanceGraphicsItem::setAlignment(Qt::Alignment alignment)
{
    alignment_ = alignment;
    label_text_item_->setTextAlignment(Qt::AlignVCenter | alignment_);

    // Remove them from the layout before re-ordering them
    main_layout_->removeItem(icon_pixmap_item_);
    main_layout_->removeItem(info_layout_);

    QGraphicsLayoutItem* first_item = nullptr;
    QGraphicsLayoutItem* second_item = nullptr;

    if (alignment == Qt::AlignLeft) {
        first_item = icon_pixmap_item_;
        second_item = info_layout_;
    } else {
        first_item = info_layout_;
        second_item = icon_pixmap_item_;
    }

    // Re-add the items in order
    main_layout_->insertItem(0, second_item);
    main_layout_->insertItem(0, first_item);

    main_layout_->setAlignment(info_layout_, Qt::AlignCenter);
    main_layout_->setAlignment(icon_pixmap_item_, Qt::AlignCenter);

    themeChanged();
}

/**
 * @brief PortInstanceGraphicsItem::playEvents
 * @param from_time
 * @param to_time
 */
void PortInstanceGraphicsItem::playEvents(qint64 from_time, qint64 to_time)
{
    const auto& port_lifecycle_event_series = port_inst_data_.getPortLifecycleSeries();
    const auto& port_lifecycle_events = port_lifecycle_event_series->getEventsBetween(from_time, to_time);
    if (!port_lifecycle_events.isEmpty()) {
        flashPort(MEDEA::ChartDataKind::PORT_LIFECYCLE, from_time);
    }

    const auto& port_event_series = port_inst_data_.getPortEventSeries();
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
        if (event_src_port_) {
            emit flashEdge(from_time, flash_duration_ms);
        }
    }
}

/**
 * @brief PortInstanceGraphicsItem::flashPort
 * @param event_kind
 * @param from_time
 * @param flash_color
 */
void PortInstanceGraphicsItem::flashPort(MEDEA::ChartDataKind event_kind, qint64 from_time, QColor flash_color)
{
    if (!flash_color.isValid()) {
        flash_color = highlight_color_;
    }

    // This function can be called multiple times by multiple callers
    // Due to this, the flash end time needs to be stored and updated to avoid the flash being stopped prematurely
    auto&& end_time = from_time + flash_duration_ms;

    // Switch the color
    if (event_kind == MEDEA::ChartDataKind::PORT_LIFECYCLE) {
        ellipse_pen_.setColor(ellipse_pen_color_);
        port_lifecycle_flash_end_time_ = qMax(end_time, port_lifecycle_flash_end_time_);
    } else if (event_kind == MEDEA::ChartDataKind::PORT_EVENT) {
        ellipse_color_ = flash_color;
        port_event_flash_end_time_ = qMax(end_time, port_event_flash_end_time_);
    }
    update();

    QTimer::singleShot(flash_duration_ms, this, [this, event_kind, end_time]() {
        unflashPort(event_kind, end_time);
    });
}

/**
 * @brief PortInstanceGraphicsItem::unflashPort
 * @param event_kind
 * @param flash_end_time
 */
void PortInstanceGraphicsItem::unflashPort(MEDEA::ChartDataKind event_kind, qint64 flash_end_time)
{
    // Reset the color when we've reached the flash end time
    if (event_kind == MEDEA::ChartDataKind::PORT_LIFECYCLE) {
        if (port_lifecycle_flash_end_time_ == flash_end_time) {
            ellipse_pen_.setColor(Qt::transparent);
            port_lifecycle_flash_end_time_ = 0;
        }
    } else if (event_kind == MEDEA::ChartDataKind::PORT_EVENT) {
        if (port_event_flash_end_time_ == flash_end_time) {
            ellipse_color_ = default_color_;
            port_event_flash_end_time_ = 0;
        }
    }
    update();
}

/**
 * @brief PortInstanceGraphicsItem::boundingRect
 * @return
 */
QRectF PortInstanceGraphicsItem::boundingRect() const
{
    return QRectF(0, 0, getWidth(), getHeight());
}

/**
 * @brief PortInstanceGraphicsItem::sizeHint
 * @param which
 * @param constraint
 * @return
 */
QSizeF PortInstanceGraphicsItem::sizeHint(Qt::SizeHint which, const QSizeF& constraint) const
{
    switch (which) {
    case Qt::MinimumSize:
    case Qt::PreferredSize:
        return boundingRect().size();
    case Qt::MaximumSize:
        return main_layout_->geometry().size();
    default:
        break;
    }
    return constraint;
}

/**
 * @brief PortInstanceGraphicsItem::paint
 * @param painter
 * @param option
 * @param widget
 */
void PortInstanceGraphicsItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    painter->setRenderHint(QPainter::Antialiasing, true);

    auto ellipse_size = icon_size / 2.0;
    painter->setPen(ellipse_pen_);
    painter->setBrush(ellipse_color_);
    painter->drawEllipse(icon_pixmap_item_->geometry().center(), ellipse_size, ellipse_size);
}

/**
 * @brief PortInstanceGraphicsItem::getWidth
 * @return
 */
qreal PortInstanceGraphicsItem::getWidth() const
{
    return main_layout_->geometry().width();
}

/**
 * @brief PortInstanceGraphicsItem::getHeight
 * @return
 */
qreal PortInstanceGraphicsItem::getHeight() const
{
    return main_layout_->geometry().height();
}

/**
 * @brief PortInstanceGraphicsItem::themeChanged
 */
void PortInstanceGraphicsItem::themeChanged()
{
    Theme* theme = Theme::theme();
    highlight_color_ = theme->getHighlightColor();

    QColor pixmap_color = theme->getMenuIconColor();
    if (alignment_ == Qt::AlignLeft) {
        default_color_ = theme->getSeverityColor(Notification::Severity::ERROR);
    } else {
        default_color_ = theme->getSeverityColor(Notification::Severity::SUCCESS);
    }

    ellipse_color_ = default_color_;
    ellipse_pen_color_ = theme->getTextColor();
    ellipse_pen_ = QPen(Qt::transparent, 3.0);

    auto pixmap = theme->getImage(icon_path.first, icon_path.second, QSize(), pixmap_color);
    icon_pixmap_item_->updatePixmap(pixmap);
    label_text_item_->setDefaultTextColor(theme->getTextColor());

    /*
    // TODO - Uncomment when we add the Aggregate message type
    pixmap = theme->getImage("Icons", "envelopeTwoTone", QSize(), theme->getAltTextColor());
    sub_icon_pixmap_item_->updatePixmap(pixmap);
    sub_label_text_item_->setDefaultTextColor(theme->getTextColor());
    */
}

/**
 * @brief PortInstanceGraphicsItem::setupLayout
 */
void PortInstanceGraphicsItem::setupLayout()
{
    QPixmap pix = Theme::theme()->getImage(icon_path.first, icon_path.second);
    icon_pixmap_item_ = new PixmapGraphicsItem(pix, this);
    icon_pixmap_item_->setPixmapPadding(pixmap_padding);
    icon_pixmap_item_->setSquareSize(icon_size);
    icon_pixmap_item_->setMaximumHeight(icon_pixmap_item_->minimumHeight());

    label_text_item_ = new TextGraphicsItem(getPortName(), this);
    label_text_item_->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);
    label_text_item_->setTextAlignment(Qt::AlignVCenter | alignment_);

    info_layout_ = new QGraphicsLinearLayout(Qt::Vertical);
    info_layout_->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);
    info_layout_->setSpacing(0);
    info_layout_->setContentsMargins(0, 0, 0, 0);
    info_layout_->addItem(label_text_item_);
    info_layout_->setAlignment(label_text_item_, Qt::AlignVCenter | alignment_);

    main_layout_ = new QGraphicsLinearLayout(Qt::Horizontal);
    main_layout_->setSpacing(10);
    main_layout_->setContentsMargins(0, 0, 0, 0);
    main_layout_->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);

    if (alignment_ == Qt::AlignLeft) {
        main_layout_->addItem(icon_pixmap_item_);
        main_layout_->addItem(info_layout_);
    } else {
        main_layout_->addItem(info_layout_);
        main_layout_->addItem(icon_pixmap_item_);
    }

    prepareGeometryChange();
    setContentsMargins(0, 0, 0, 0);
    setLayout(main_layout_);
    //setupSubInfoLayout();
}

/**
 * @brief PortInstanceGraphicsItem::setupSubInfoLayout
 */
void PortInstanceGraphicsItem::setupSubInfoLayout()
{
    int sub_size = icon_size / 2;

    QPixmap pix = Theme::theme()->getImage("Icons", "envelopeTwoTone");
    sub_icon_pixmap_item_ = new PixmapGraphicsItem(pix, this);
    sub_icon_pixmap_item_->setSquareSize(sub_size);

    sub_label_text_item_ = new TextGraphicsItem("Message", this);
    sub_label_text_item_->setFont(QFont("Verdana", 8));
    sub_label_text_item_->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

    sub_info_layout_ = new QGraphicsLinearLayout(Qt::Horizontal);
    sub_info_layout_->setSpacing(5);
    sub_info_layout_->setContentsMargins(0, 0, 0, 0);
    sub_info_layout_->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

    sub_info_layout_->addItem(sub_icon_pixmap_item_);
    sub_info_layout_->setAlignment(sub_icon_pixmap_item_, Qt::AlignCenter);
    sub_info_layout_->setStretchFactor(sub_icon_pixmap_item_, 0);

    sub_info_layout_->addItem(sub_label_text_item_);
    sub_info_layout_->setAlignment(sub_label_text_item_, Qt::AlignLeft);
    sub_info_layout_->setStretchFactor(sub_label_text_item_, 1);

    info_layout_->addItem(sub_info_layout_);
}