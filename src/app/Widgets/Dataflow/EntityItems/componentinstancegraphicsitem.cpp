#include "componentinstancegraphicsitem.h"
#include "../../../theme.h"

#include <QGraphicsSceneMouseEvent>
#include <QPainter>

#define PEN_WIDTH 2.0


/**
 * @brief ComponentInstanceGraphicsItem::ComponentInstanceGraphicsItem
 * @param comp_inst_data
 * @param parent
 */
ComponentInstanceGraphicsItem::ComponentInstanceGraphicsItem(const ComponentInstanceData& comp_inst_data, QGraphicsItem* parent)
    : QGraphicsWidget(parent),
      comp_inst_name_(comp_inst_data.getName()),
      comp_inst_graphml_id_(comp_inst_data.getGraphmlID())
{
    setFlags(flags() | QGraphicsWidget::ItemIsMovable | QGraphicsWidget::ItemIsSelectable);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    setupLayout();
    themeChanged();

    connect(Theme::theme(), &Theme::theme_Changed, this, &ComponentInstanceGraphicsItem::themeChanged);
}


/**
 * @brief ComponentInstanceGraphicsItem::addPortInstanceItem
 * @param port_data
 */
PortInstanceGraphicsItem* ComponentInstanceGraphicsItem::addPortInstanceItem(PortInstanceData& port_data)
{
    auto port_inst_item = new PortInstanceGraphicsItem(port_data);
    connect(this, &ComponentInstanceGraphicsItem::itemMoved, port_inst_item, &PortInstanceGraphicsItem::itemMoved);

    // If this is the first port item, the children layout still needs to be constructed
    if (children_layout_ == nullptr) {
        auto spacing = MEDEA::GraphicsLayoutItem::DEFAULT_GRAPHICS_ITEM_HEIGHT * 0.75;
        auto margin = spacing / 4;
        children_layout_ = new QGraphicsGridLayout;
        children_layout_->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
        children_layout_->setContentsMargins(margin, margin - 5, margin, margin);
        children_layout_->setHorizontalSpacing(spacing);
        children_layout_->setVerticalSpacing(spacing / 2);
        main_layout_->addItem(children_layout_);
    }

    prepareGeometryChange();

    auto&& port_kind = port_inst_item->getPortKind();
    if (port_kind == AggServerResponse::Port::Kind::PUBLISHER || port_kind == AggServerResponse::Port::Kind::REQUESTER) {
        auto&& alignment = Qt::AlignRight;
        port_inst_item->setAlignment(alignment);
        children_layout_->addItem(port_inst_item, right_port_count_, 1, alignment);
        right_port_count_++;
    } else {
        auto&& alignment = Qt::AlignLeft;
        port_inst_item->setAlignment(alignment);
        children_layout_->addItem(port_inst_item, left_port_count_, 0, alignment);
        left_port_count_++;
    }

    return port_inst_item;
}


/**
 * @brief ComponentInstanceGraphicsItem::boundingRect
 * @return
 */
QRectF ComponentInstanceGraphicsItem::boundingRect() const
{
    QRectF rect(0, 0, geometry().width(), geometry().height());
    auto padding = PEN_WIDTH / 2.0;
    return rect.adjusted(-padding, -padding, padding, padding);
}


/**
 * @brief ComponentInstanceGraphicsItem::paint
 * @param painter
 * @param option
 * @param widget
 */
void ComponentInstanceGraphicsItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    auto padding = PEN_WIDTH / 2.0;
    QRectF rect = boundingRect().adjusted(padding, padding, -padding, -padding);
    painter->fillRect(rect, body_color_);
    painter->fillRect(top_layout_->geometry(), top_color_);

    painter->setPen(QPen(top_color_, 2));
    painter->setBrush(Qt::NoBrush);
    painter->drawRoundedRect(rect, 2, 2);
}


/**
 * @brief ComponentInstanceGraphicsItem::mousePressEvent
 * @param event
 */
void ComponentInstanceGraphicsItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        prev_move_origin_ = mapToScene(event->pos());
        moving_ = true;
    }

    QGraphicsWidget::mousePressEvent(event);
}


/**
 * @brief ComponentInstanceGraphicsItem::mouseMoveEvent
 * @param event
 */
void ComponentInstanceGraphicsItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if (moving_) {
        auto cursor_pos = mapToScene(event->pos());
        auto delta = cursor_pos - prev_move_origin_;
        prev_move_origin_ = cursor_pos;
        moveBy(delta.x(), delta.y());
    }

    QGraphicsWidget::mouseMoveEvent(event);
}


/**
 * @brief ComponentInstanceGraphicsItem::mouseReleaseEvent
 * @param event
 */
void ComponentInstanceGraphicsItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    moving_ = false;
    prev_move_origin_ = QPointF(0, 0);

    QGraphicsWidget::mouseReleaseEvent(event);
}


/**
 * @brief ComponentInstanceGraphicsItem::mouseDoubleClickEvent
 * @param event
 */
void ComponentInstanceGraphicsItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    // TODO - Toggle children ports info (MED-730)
    QGraphicsWidget::mouseDoubleClickEvent(event);
}


/**
 * @brief ComponentInstanceGraphicsItem::moveEvent
 * @param event
 */
void ComponentInstanceGraphicsItem::moveEvent(QGraphicsSceneMoveEvent *event)
{
    emit itemMoved();
    QGraphicsWidget::moveEvent(event);
}


/**
 * @brief ComponentInstanceGraphicsItem::themeChanged
 */
void ComponentInstanceGraphicsItem::themeChanged()
{
    Theme* theme = Theme::theme();

    auto pixmap = theme->getImage("EntityIcons", "ComponentInstance");
    icon_pixmap_item_->updatePixmap(pixmap);
    label_text_item_->setDefaultTextColor(theme->getTextColor());

    top_color_ = theme->getActiveWidgetBorderColor();
    body_color_ = theme->getAltBackgroundColor();
    update();
}   


/**
 * @brief ComponentInstanceGraphicsItem::setupLayout
 */
void ComponentInstanceGraphicsItem::setupLayout()
{
    QPixmap pix = Theme::theme()->getImage("EntityIcons", "ComponentInstance");
    icon_pixmap_item_ = new PixmapGraphicsItem(pix, this);
    label_text_item_ = new TextGraphicsItem(comp_inst_name_ + " [" + comp_inst_graphml_id_ + "]", this);

    icon_pixmap_item_->setParentItem(this);
    label_text_item_->setParentItem(this);

    top_layout_ = new QGraphicsLinearLayout(Qt::Horizontal);
    top_layout_->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);
    top_layout_->setSpacing(5);
    top_layout_->setContentsMargins(10, 0, 10, 0);
    top_layout_->addItem(icon_pixmap_item_);
    top_layout_->setStretchFactor(icon_pixmap_item_, 0);
    top_layout_->addItem(label_text_item_);
    top_layout_->setStretchFactor(label_text_item_, 1);

    main_layout_ = new QGraphicsLinearLayout(Qt::Vertical);
    main_layout_->setContentsMargins(0, 0, 0, 0);
    main_layout_->setSpacing(0);
    main_layout_->addItem(top_layout_);
    main_layout_->setStretchFactor(top_layout_, 0);

    prepareGeometryChange();
    setContentsMargins(0, 0, 0, 0);
    setLayout(main_layout_);
}

