#include "componentinstancegraphicsitem.h"
#include "nodegraphicsitem.h"
#include "../../../theme.h"

#include <QPainter>
#include <QGraphicsSceneMouseEvent>

const qreal pen_width = 2.0;
const int icon_size = 50;
const int top_layout_spacing = 5;
const int top_layout_horizontal_margin = 10;

/**
 * @brief ComponentInstanceGraphicsItem::ComponentInstanceGraphicsItem
 * @param comp_inst_data
 * @param parent
 */
ComponentInstanceGraphicsItem::ComponentInstanceGraphicsItem(const ComponentInstanceData& comp_inst_data, NodeGraphicsItem* parent)
    : QGraphicsWidget(parent),
      comp_inst_data_(comp_inst_data)
{
    setupLayout();
    setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    setFlags(flags() | QGraphicsWidget::ItemIsMovable | QGraphicsWidget::ItemIsSelectable);
    setAcceptedMouseButtons(Qt::LeftButton);

    connect(this, &ComponentInstanceGraphicsItem::geometryChanged, this, &ComponentInstanceGraphicsItem::updateConnectionPos);
    connect(Theme::theme(), &Theme::theme_Changed, this, &ComponentInstanceGraphicsItem::themeChanged);
    themeChanged();
}

/**
 * @brief ComponentInstanceGraphicsItem::addPortInstanceItem
 * @param port_data
 */
PortInstanceGraphicsItem* ComponentInstanceGraphicsItem::addPortInstanceItem(PortInstanceData& port_data)
{
    auto port_inst_item = new PortInstanceGraphicsItem(port_data, this);
    connect(this, &ComponentInstanceGraphicsItem::visibleChanged, port_inst_item, &PortInstanceGraphicsItem::visibleChanged);
    connect(this, &ComponentInstanceGraphicsItem::updateConnectionPos, port_inst_item, &PortInstanceGraphicsItem::updateConnectionPos);

    port_inst_item->setParentItem(this);
    port_inst_items_.push_back(port_inst_item);
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
    QRectF rect(0, 0, getWidth(), getHeight());
    auto padding = pen_width / 2.0;
    return rect.adjusted(-padding, -padding, padding, padding);
}

/**
 * @brief ComponentInstanceGraphicsItem::sizeHint
 * @param which
 * @param constraint
 * @return
 */
QSizeF ComponentInstanceGraphicsItem::sizeHint(Qt::SizeHint which, const QSizeF& constraint) const
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
 * @brief ComponentInstanceGraphicsItem::paint
 * @param painter
 * @param option
 * @param widget
 */
void ComponentInstanceGraphicsItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    auto padding = pen_width / 2.0;
    QRectF rect = boundingRect().adjusted(padding, padding, -padding, -padding);
    painter->fillRect(rect, body_color_);

    auto top_rect = top_layout_->geometry();
    top_rect.setWidth(rect.width());
    painter->fillRect(top_rect, top_color_);

    painter->setPen(QPen(top_color_, 2));
    painter->drawRoundedRect(rect, 2, 2);
}

/**
 * @brief ComponentInstanceGraphicsItem::mousePressEvent
 * @param event
 */
void ComponentInstanceGraphicsItem::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    if (top_layout_->geometry().contains(event->pos())) {
        prev_move_origin_ = mapToScene(event->pos());
        moving_ = true;
    }
    QGraphicsWidget::mousePressEvent(event);
}

/**
 * @brief ComponentInstanceGraphicsItem::mouseMoveEvent
 * @param event
 */
void ComponentInstanceGraphicsItem::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
    if (moving_) {
        auto cursor_pos = mapToScene(event->pos());
        auto delta = cursor_pos - prev_move_origin_;
        prev_move_origin_ = cursor_pos;
        emit attemptMove(this, pos() + delta);
    }
}

/**
 * @brief ComponentInstanceGraphicsItem::mouseReleaseEvent
 * @param event
 */
void ComponentInstanceGraphicsItem::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    moving_ = false;
    prev_move_origin_ = QPointF(0, 0);
    QGraphicsWidget::mouseReleaseEvent(event);
}

/**
 * @brief ComponentInstanceGraphicsItem::mouseDoubleClickEvent
 * @param event
 */
void ComponentInstanceGraphicsItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event)
{
    if (toggle_pixmap_item_->geometry().contains(event->pos())) {
        // TODO - Work out how to update the port inst item's geometry correctly when things inside it are shown/hidden
        //toggleDetailsVisible();
    } else if (icon_pixmap_item_->geometry().contains(event->pos())) {
        toggleExpanded();
    }
    QGraphicsWidget::mouseDoubleClickEvent(event);
}

/**
 * @brief ComponentInstanceGraphicsItem::toggleExpanded
 */
void ComponentInstanceGraphicsItem::toggleExpanded()
{
    expanded_ = !expanded_;
    prepareGeometryChange();

    for (const auto& port_inst : port_inst_items_) {
        port_inst->setVisible(expanded_);
    }

    update();
    emit itemExpanded(expanded_);
}

/**
 * @brief ComponentInstanceGraphicsItem::moveTo
 * @param x
 * @param y
 */
void ComponentInstanceGraphicsItem::moveTo(int x, int y)
{
    prepareGeometryChange();
    setPos(x, y);
    update();
}

/**
 * @brief ComponentInstanceGraphicsItem::getWidth
 * @return
 */
qreal ComponentInstanceGraphicsItem::getWidth() const
{
    if (expanded_) {
        return main_layout_->geometry().width();
    } else {
        auto contracted_width = icon_pixmap_item_->geometry().width() + label_text_item_->minimumWidth();
        contracted_width += top_layout_spacing + top_layout_horizontal_margin * 2;
        return contracted_width;
    }
}

/**
 * @brief ComponentInstanceGraphicsItem::getHeight
 * @return
 */
qreal ComponentInstanceGraphicsItem::getHeight() const
{
    if (expanded_) {
        return main_layout_->geometry().height();
    } else {
        return top_layout_->geometry().height();
    }
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

    pixmap = theme->getImage("Icons", "circleInfoDark");
    toggle_pixmap_item_->updatePixmap(pixmap);

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
    label_text_item_ = new TextGraphicsItem(comp_inst_data_.getName() + " [" + comp_inst_data_.getGraphmlID() + "]", this);

    icon_pixmap_item_->setParentItem(this);
    label_text_item_->setParentItem(this);

    pix = Theme::theme()->getImage("Icons", "circleInfoDark");
    toggle_pixmap_item_ = new PixmapGraphicsItem(pix, this);
    toggle_pixmap_item_->setParentItem(this);
    toggle_pixmap_item_->setPixmapPadding(5);
    toggle_pixmap_item_->setSquareSize(icon_size);
    toggle_pixmap_item_->setVisible(false);

    top_layout_ = new QGraphicsLinearLayout(Qt::Horizontal);
    top_layout_->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
    top_layout_->setSpacing(top_layout_spacing);
    top_layout_->setContentsMargins(top_layout_horizontal_margin, 0, top_layout_horizontal_margin, 0);

    top_layout_->addItem(icon_pixmap_item_);
    top_layout_->setStretchFactor(icon_pixmap_item_, 0);

    top_layout_->addItem(label_text_item_);
    top_layout_->setStretchFactor(label_text_item_, 1);

    //top_layout_->addItem(toggle_pixmap_item_);
    //top_layout_->setStretchFactor(toggle_pixmap_item_, 0);

    auto spacing = MEDEA::GraphicsLayoutItem::DEFAULT_GRAPHICS_ITEM_HEIGHT * 0.75;
    auto margin = spacing / 4;
    children_layout_ = new QGraphicsGridLayout;
    children_layout_->setContentsMargins(margin, margin, margin, margin);
    children_layout_->setHorizontalSpacing(spacing);
    children_layout_->setVerticalSpacing(margin);

    main_layout_ = new QGraphicsLinearLayout(Qt::Vertical);
    main_layout_->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    main_layout_->setContentsMargins(0, 0, 0, 0);
    main_layout_->setSpacing(0);
    main_layout_->addItem(top_layout_);
    main_layout_->setStretchFactor(top_layout_, 0);
    main_layout_->addItem(children_layout_);
    main_layout_->setStretchFactor(children_layout_, 1);

    prepareGeometryChange();
    setContentsMargins(0, 0, 0, 0);
    setLayout(main_layout_);
}