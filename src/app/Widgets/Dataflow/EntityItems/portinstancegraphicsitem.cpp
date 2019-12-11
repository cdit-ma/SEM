#include "portinstancegraphicsitem.h"
#include "../../../theme.h"

#include <QPainter>
#include <QTimer>

//#define PIXMAP_PADDING 15 // better for square icons
#define PIXMAP_PADDING 10

/**
 * @brief PortInstanceGraphicsItem::PortInstanceGraphicsItem
 * @param port_data
 * @param parent
 */
PortInstanceGraphicsItem::PortInstanceGraphicsItem(const PortInstanceData& port_data, QGraphicsItem* parent)
    : QGraphicsWidget(parent),
      graphml_id_(port_data.getGraphmlID()),
      port_name_(port_data.getName()),
      port_kind_(port_data.getKind())
{
    icon_path.first = "Icons";

    // Identify Port kind
    switch (port_kind_) {
    case AggServerResponse::Port::Kind::PERIODIC: {
        icon_path.second = "clockCycle";
        break;
    }
    case AggServerResponse::Port::Kind::PUBLISHER: {
        icon_path.second = "arrowRightLong";
        break;
    }
    case AggServerResponse::Port::Kind::SUBSCRIBER: {
        icon_path.second = "arrowIntoBox";
        break;
    }
    case AggServerResponse::Port::Kind::REQUESTER: {
        icon_path.second = "arrowTopRight";
        break;
    }
    case AggServerResponse::Port::Kind::REPLIER: {
        icon_path.second = "arrowBottomRight";
        break;
    }
    default:
        break;
    }

    setFlags(flags() | QGraphicsWidget::ItemIsSelectable);
    setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);

    setupCentralisedIconLayout();

    connect(Theme::theme(), &Theme::theme_Changed, this, &PortInstanceGraphicsItem::themeChanged);
    themeChanged();
}


/**
 * @brief PortInstanceGraphicsItem::getGraphmlID
 * @return
 */
const QString& PortInstanceGraphicsItem::getGraphmlID() const
{
    return graphml_id_;
}


/**
 * @brief PortInstanceGraphicsItem::getPortName
 * @return
 */
const QString& PortInstanceGraphicsItem::getPortName() const
{
    return port_name_;
}


/**
 * @brief PortInstanceGraphicsItem::getPortKind
 * @return
 */
AggServerResponse::Port::Kind PortInstanceGraphicsItem::getPortKind() const
{
    return port_kind_;
}


/**
 * @brief PortInstanceGraphicsItem::getIconSceneRect
 * @return
 */
QRectF PortInstanceGraphicsItem::getIconSceneRect() const
{
    //return mapFromParent(icon_pixmap_item_->boundingRect()).boundingRect();
    return icon_pixmap_item_->sceneBoundingRect();
}


/**
 * @brief PortInstanceGraphicsItem::setAlignment
 * @param alignment
 */
void PortInstanceGraphicsItem::setAlignment(Qt::Alignment alignment)
{
    alignment_ = alignment;

    // Remove them from the layout before re-ordering them
    main_layout_->removeItem(icon_pixmap_item_);
    main_layout_->removeItem(top_layout_);

    QGraphicsLayoutItem* first_item = nullptr;
    QGraphicsLayoutItem* second_item = nullptr;

    if (alignment == Qt::AlignLeft) {
        first_item = icon_pixmap_item_;
        second_item = top_layout_;
    } else {
        first_item = top_layout_;
        second_item = icon_pixmap_item_;
    }

    // Re-add the items in order
    main_layout_->insertItem(0, second_item);
    main_layout_->insertItem(0, first_item);

    main_layout_->setAlignment(top_layout_, Qt::AlignCenter);
    main_layout_->setAlignment(icon_pixmap_item_, Qt::AlignCenter);
    themeChanged();
}


/**
 * @brief PortInstanceGraphicsItem::flashPort
 * @param start_time
 * @param flash_duration_ms
 * @param flash_color
 */
void PortInstanceGraphicsItem::flashPort(qint64 start_time, qint64 flash_duration_ms, QColor flash_color)
{
    if (flash_duration_ms <= 0) {
        return;
    }

    if (!flash_color.isValid()) {
        flash_color = highlight_color_;
    }

    // Switch the ellipse color
    ellipse_color_ = flash_color;
    update();

    // This function can be called multiple times by multiple callers
    // Due to this, the flash end time needs to be stored and updated to avoid the flash being stopped prematurely
    auto&& end_time = start_time + flash_duration_ms;
    flash_end_time_ = qMax(end_time, flash_end_time_);

    QTimer::singleShot(static_cast<int>(flash_duration_ms), this, [this, end_time]() {
        unflashPort(end_time);
    });
}


/**
 * @brief PortInstanceGraphicsItem::unflashPort
 * @param flash_end_time
 */
void PortInstanceGraphicsItem::unflashPort(qint64 flash_end_time)
{
    if (flash_end_time_ == flash_end_time) {
        // Reset the ellipse color
        ellipse_color_ = default_color_;
        update();
        flash_end_time_ = 0;
    }
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
    painter->setPen(Qt::NoPen);

    auto icon_size = icon_pixmap_item_->preferredWidth() / 2.0;
    painter->setBrush(ellipse_color_);
    painter->drawEllipse(icon_pixmap_item_->geometry().center(), icon_size, icon_size);
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

    auto pixmap = theme->getImage(icon_path.first, icon_path.second, QSize(), pixmap_color);
    icon_pixmap_item_->updatePixmap(pixmap);
    label_text_item_->setDefaultTextColor(theme->getTextColor());

    pixmap = theme->getImage("Icons", "envelopeTwoTone", QSize(), theme->getAltTextColor());
    sub_icon_pixmap_item_->updatePixmap(pixmap);
    sub_label_text_item_->setDefaultTextColor(theme->getTextColor());
}


/**
 * @brief PortInstanceGraphicsItem::setupCentralisedIconLayout
 */
void PortInstanceGraphicsItem::setupCentralisedIconLayout()
{
    int size = 50;

    QPixmap pix = Theme::theme()->getImage(icon_path.first, icon_path.second);
    icon_pixmap_item_ = new PixmapGraphicsItem(pix, this);
    icon_pixmap_item_->setPixmapPadding(PIXMAP_PADDING);
    icon_pixmap_item_->setParentItem(this);
    icon_pixmap_item_->setSquareSize(size);

    label_text_item_ = new TextGraphicsItem(port_name_, this);
    label_text_item_->setParentItem(this);

    int sub_size = size / 2;

    pix = Theme::theme()->getImage("Icons", "envelopeTwoTone");
    sub_icon_pixmap_item_ = new PixmapGraphicsItem(pix, this);
    sub_icon_pixmap_item_->setParentItem(this);
    sub_icon_pixmap_item_->setSquareSize(sub_size);

    sub_label_text_item_ = new TextGraphicsItem("Message", this);
    sub_label_text_item_->setFont(QFont("Verdana", 8));
    sub_label_text_item_->setParentItem(this);
    sub_label_text_item_->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Preferred);

    children_layout_ = new QGraphicsLinearLayout(Qt::Horizontal);
    children_layout_->setSpacing(5);
    children_layout_->setContentsMargins(0, 0, 0, 0);
    children_layout_->addItem(sub_icon_pixmap_item_);
    children_layout_->addItem(sub_label_text_item_);
    children_layout_->setAlignment(sub_icon_pixmap_item_, Qt::AlignCenter);
    children_layout_->setAlignment(sub_label_text_item_, Qt::AlignLeft);
    children_layout_->setStretchFactor(sub_icon_pixmap_item_, 0);
    children_layout_->setStretchFactor(sub_label_text_item_, 1);

    top_layout_ = new QGraphicsLinearLayout(Qt::Vertical);
    top_layout_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
    top_layout_->setSpacing(0);
    top_layout_->setContentsMargins(0, 0, 0, 0);
    top_layout_->addItem(label_text_item_);
    top_layout_->addItem(children_layout_);
    top_layout_->setAlignment(label_text_item_, Qt::AlignVCenter | alignment_);
    label_text_item_->setTextAlignment(Qt::AlignVCenter | alignment_);

    main_layout_ = new QGraphicsLinearLayout(Qt::Horizontal);
    main_layout_->setSpacing(10);
    main_layout_->setContentsMargins(0, 0, 0, 0);

    if (alignment_ == Qt::AlignLeft) {
        main_layout_->addItem(icon_pixmap_item_);
        main_layout_->addItem(top_layout_);
    } else {
        main_layout_->addItem(top_layout_);
        main_layout_->addItem(icon_pixmap_item_);
    }

    main_layout_->setAlignment(top_layout_, Qt::AlignCenter);
    main_layout_->setAlignment(icon_pixmap_item_, Qt::AlignCenter);

    prepareGeometryChange();
    setContentsMargins(0, 0, 0, 0);
    setLayout(main_layout_);
}
