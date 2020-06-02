#include "nodegraphicsitem.h"
#include "../../../theme.h"

#include <QPainter>
#include <QGraphicsSceneMouseEvent>

const qreal pen_width = 2.0;
const int icon_size = 50;

const int padding = 15;
const int top_layout_spacing = 5;
const int top_layout_horizontal_margin = 5;

/**
 * @brief NodeGraphicsItem::NodeGraphicsItem
 * @param node_data
 * @param parent
 */
NodeGraphicsItem::NodeGraphicsItem(const NodeData& node_data, QGraphicsItem* parent)
    : QGraphicsWidget(parent),
      node_data_(node_data)
{
    setupLayout();
    setFlags(flags() | QGraphicsWidget::ItemIsMovable | QGraphicsWidget::ItemIsSelectable);
    setAcceptedMouseButtons(Qt::LeftButton);

    connect(this, &NodeGraphicsItem::geometryChanged, this, &NodeGraphicsItem::updateConnectionPos);
    connect(Theme::theme(), &Theme::theme_Changed, this, &NodeGraphicsItem::themeChanged);
    themeChanged();

    constructChildrenItems();
}

/**
 * @brief NodeGraphicsItem::addComponentInstanceItem
 * @param comp_inst_data
 * @return
 */
ComponentInstanceGraphicsItem* NodeGraphicsItem::addComponentInstanceItem(ComponentInstanceData& comp_inst_data)
{
    auto comp_inst_item = new ComponentInstanceGraphicsItem(comp_inst_data, this);
    connect(this, &NodeGraphicsItem::updateConnectionPos, comp_inst_item, &ComponentInstanceGraphicsItem::updateConnectionPos);
    connect(comp_inst_item, &ComponentInstanceGraphicsItem::geometryChanged, this, &NodeGraphicsItem::updateOnGeometryChange);
    connect(comp_inst_item, &ComponentInstanceGraphicsItem::itemExpanded, this, &NodeGraphicsItem::updateOnGeometryChange);
    connect(comp_inst_item, &ComponentInstanceGraphicsItem::attemptMove, this, &NodeGraphicsItem::validateChildMove);
    prepareGeometryChange();

    // Set initial position
    auto&& child_pos = getAvailableChildPos();
    comp_inst_item->setPos(child_pos.x(), child_pos.y());
    comp_inst_item->setParentItem(this);
    comp_inst_items_.push_back(comp_inst_item);

    updateOnGeometryChange();
    return comp_inst_item;
}

/**
 * @brief NodeGraphicsItem::getComponentInstanceItems
 * @return
 */
const std::vector<ComponentInstanceGraphicsItem*>& NodeGraphicsItem::getComponentInstanceItems() const
{
    return comp_inst_items_;
}


/**
 * @brief NodeGraphicsItem::boundingRect
 * @return
 */
QRectF NodeGraphicsItem::boundingRect() const
{
    QRectF rect(0, 0, getWidth(), getHeight());
    auto rect_padding = pen_width / 2.0;
    return rect.adjusted(-rect_padding, -rect_padding, rect_padding, rect_padding);
}

/**
 * @brief NodeGraphicsItem::sizeHint
 * @param which
 * @param constraint
 * @return
 */
QSizeF NodeGraphicsItem::sizeHint(Qt::SizeHint which, const QSizeF& constraint) const
{
    switch (which) {
        case Qt::MinimumSize:
        case Qt::PreferredSize:
            return boundingRect().size();
        case Qt::MaximumSize:
            return {100000, 10000};
        default:
            break;
    }
    return constraint;
}

/**
 * @brief NodeGraphicsItem::setGeometry
 * @param rect
 */
void NodeGraphicsItem::setGeometry(const QRectF& rect)
{
    prepareGeometryChange();

    // Force this item's geometry to have the same size as the bounding rect
    auto&& bounding_size = boundingRect().size();
    setMinimumSize(bounding_size);
    setMaximumSize(bounding_size);
    setPreferredSize(bounding_size);

    auto adjusted_rect = rect;
    adjusted_rect.setSize(bounding_size);
    QGraphicsWidget::setGeometry(adjusted_rect);
}

/**
 * @brief NodeGraphicsItem::paint
 * @param painter
 * @param option
 * @param widget
 */
void NodeGraphicsItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    auto rect_padding = pen_width / 2.0;
    QRectF rect = boundingRect().adjusted(rect_padding, rect_padding, -rect_padding, -rect_padding);
    painter->fillRect(rect, body_color_);
    painter->fillRect(getTopRect(), top_color_);

    painter->setPen(QPen(top_color_, pen_width));
    painter->drawRoundedRect(rect, 2, 2);
}

/**
 * @brief NodeGraphicsItem::mousePressEvent
 * @param event
 */
void NodeGraphicsItem::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    if (getTopRect().contains(event->pos())) {
        prev_move_origin_ = mapToScene(event->pos());
        moving_ = true;
    }
    QGraphicsWidget::mousePressEvent(event);
}

/**
 * @brief NodeGraphicsItem::mouseMoveEvent
 * @param event
 */
void NodeGraphicsItem::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
    if (moving_) {
        auto cursor_pos = mapToScene(event->pos());
        auto delta = cursor_pos - prev_move_origin_;
        prev_move_origin_ = cursor_pos;
        moveBy(delta.x(), delta.y());
        emit updateConnectionPos();
    }
}

/**
 * @brief NodeGraphicsItem::mouseReleaseEvent
 * @param event
 */
void NodeGraphicsItem::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    moving_ = false;
    prev_move_origin_ = QPointF(0, 0);
    QGraphicsWidget::mouseReleaseEvent(event);
}

/**
 * @brief NodeGraphicsItem::mouseDoubleClickEvent
 * @param event
 */
void NodeGraphicsItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event)
{
    if (icon_pixmap_item_->geometry().contains(event->pos())) {
        toggleExpanded();
    }
    QGraphicsWidget::mouseDoubleClickEvent(event);
}

/**
 * @brief NodeGraphicsItem::constructChildrenItems
 * @throws std::invalid_argument
 */
void NodeGraphicsItem::constructChildrenItems()
{
    for (const auto& container_data : node_data_.getContainerInstanceData()) {
        for (const auto& comp_inst_data : container_data->getComponentInstanceData()) {
            if (comp_inst_data == nullptr) {
                throw std::invalid_argument("NodeGraphicsItem::constructChildrenItems - ComponentInstanceData is null.");
            }
            addComponentInstanceItem(*comp_inst_data);
        }
    }
}


/**
 * @brief NodeGraphicsItem::validateChildMove
 * @param child
 * @param pos
 */
void NodeGraphicsItem::validateChildMove(ComponentInstanceGraphicsItem* child, QPointF pos)
{
    if (child != nullptr) {
        // TODO: Re-check these review comments again once later when the other branches has been merged in
        //  They may have already been fixed elsewhere
        // REVIEW (Jackson): Is there a particular reason for chosing the top left as minimum?
        //  This prevents a left/upward drag from expanding the NodeGraphicsItem, intentional?
        auto&& min_x = getTopLeftChildPos().x();
        auto&& min_y = getTopLeftChildPos().y();
        auto x = pos.x();
        auto y = pos.y();
        if (x < min_x) {
            x = min_x;
        }
        if (y < min_y) {
            y = min_y;
        }
        prepareGeometryChange();
        child->moveTo(x, y);
        update();
    }
}

/**
 * @brief NodeGraphicsItem::toggleExpanded
 */
void NodeGraphicsItem::toggleExpanded()
{
    expanded_ = !expanded_;
    prepareGeometryChange();
    for (const auto& comp_inst : comp_inst_items_) {
        comp_inst->setVisible(expanded_);
    }
    updateOnGeometryChange();
    emit updateConnectionPos();
}

/**
 * @brief NodeGraphicsItem::updateOnGeometryChange
 */
void NodeGraphicsItem::updateOnGeometryChange()
{
    updateGeometry();
    update();
}

/**
 * @brief NodeGraphicsItem::getWidth
 * @return
 */
qreal NodeGraphicsItem::getWidth() const
{
    if (expanded_) {
        return getVisibleChildrenRect().right() + padding;
    } else {
        auto contracted_width = icon_pixmap_item_->geometry().width() + label_text_item_->minimumWidth();
        contracted_width += top_layout_spacing + top_layout_horizontal_margin * 2 + padding;
        return contracted_width;
    }
}

/**
 * @brief NodeGraphicsItem::getHeight
 * @return
 */
qreal NodeGraphicsItem::getHeight() const
{
    if (expanded_) {
        return getVisibleChildrenRect().bottom() + padding;
    } else {
        return getTopRect().height();
    }
}

/**
 * @brief NodeGraphicsItem::getTopRect
 * @return
 */
QRectF NodeGraphicsItem::getTopRect() const
{
    return QRectF(0, 0, getWidth(), MEDEA::GraphicsLayoutItem::DEFAULT_GRAPHICS_ITEM_HEIGHT);
}

/**
 * @brief NodeGraphicsItem::getVisibleChildrenRect
 * @return
 */
QRectF NodeGraphicsItem::getVisibleChildrenRect() const
{
    QRectF visible_rect;
    for (const auto& child_item : childItems()) {
        if (child_item->isVisible()) {
            auto&& child_geom = QRectF(child_item->pos(), child_item->boundingRect().size());
            visible_rect = visible_rect.united(child_geom);
        }
    }
    return visible_rect;
}

/**
 * @brief NodeGraphicsItem::getTopLeftChildPos
 * @return
 */
QPointF NodeGraphicsItem::getTopLeftChildPos() const
{
    return QPointF(padding, getTopRect().bottom() + padding);
}

/**
 * @brief NodeGraphicsItem::getAvailableChildPos
 * @return
 */
QPointF NodeGraphicsItem::getAvailableChildPos() const
{
    return QPointF(padding, childrenBoundingRect().bottom() + padding);
}

/**
 * @brief NodeGraphicsItem::themeChanged
 */
void NodeGraphicsItem::themeChanged()
{
    Theme* theme = Theme::theme();

    auto pixmap = theme->getImage("EntityIcons", "HardwareNode");
    icon_pixmap_item_->updatePixmap(pixmap);
    label_text_item_->setDefaultTextColor(theme->getTextColor());

    top_color_ = theme->getActiveWidgetBorderColor();
    body_color_ = theme->getDisabledBackgroundColor();
    update();
}

/**
 * @brief NodeGraphicsItem::setupLayout
 */
void NodeGraphicsItem::setupLayout()
{    
    QPixmap pix = Theme::theme()->getImage("EntityIcons", "HardwareNode");
    icon_pixmap_item_ = new PixmapGraphicsItem(pix, this);
    icon_pixmap_item_->setPixmapPadding(padding);
    icon_pixmap_item_->setParentItem(this);

    label_text_item_ = new TextGraphicsItem(node_data_.getHostname(), this);
    label_text_item_->setParentItem(this);

    top_layout_ = new QGraphicsLinearLayout(Qt::Horizontal);
    top_layout_->setSpacing(top_layout_spacing);
    top_layout_->setContentsMargins(top_layout_horizontal_margin, 0, top_layout_horizontal_margin, 0);
    top_layout_->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
    top_layout_->addItem(icon_pixmap_item_);
    top_layout_->setStretchFactor(icon_pixmap_item_, 0);
    top_layout_->addItem(label_text_item_);
    top_layout_->setStretchFactor(label_text_item_, 1);

    main_layout_ = new QGraphicsLinearLayout(Qt::Vertical);
    main_layout_->setContentsMargins(0, 0, 0, 0);
    main_layout_->setSpacing(0);
    main_layout_->addItem(top_layout_);
    main_layout_->setStretchFactor(top_layout_, 0);
    main_layout_->addStretch();

    prepareGeometryChange();
    setContentsMargins(0, 0, 0, 0);
    setLayout(main_layout_);
}