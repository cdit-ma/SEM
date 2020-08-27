#include "nodegraphicsitem.h"
#include "../../../theme.h"

#include <QPainter>
#include <QGraphicsSceneMouseEvent>

const qreal pen_width = 2.0;
const int icon_size = 50;

const int padding = 15;
const int pixmap_padding = 10;
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
    constructChildrenItems();

    setFlags(flags() | QGraphicsWidget::ItemIsMovable);
    setAcceptedMouseButtons(Qt::LeftButton);

    connect(Theme::theme(), &Theme::theme_Changed, this, &NodeGraphicsItem::themeChanged);
    themeChanged();
}

/**
 * @brief NodeGraphicsItem::addContainerInstanceItem
 * @param container_inst_data
 * @return
 */
void NodeGraphicsItem::addContainerInstanceItem(ContainerInstanceData& container_inst_data)
{
    // Get the initial position before constructing the child graphics item
    auto child_pos = getNextChildPos();
    auto container_inst = new ContainerInstanceGraphicsItem(container_inst_data, this);
    container_inst->setPos(child_pos.x(), child_pos.y());
    container_inst_items_.push_back(container_inst);

    connect(container_inst, &ContainerInstanceGraphicsItem::geometryChanged, this, &NodeGraphicsItem::updateGeometry);
    connect(container_inst, &ContainerInstanceGraphicsItem::requestMove, this, &NodeGraphicsItem::validateChildMove);
    connect(this, &NodeGraphicsItem::updateConnectionPos, container_inst, &ContainerInstanceGraphicsItem::updateConnectionPos);
}

/**
 * @brief NodeGraphicsItem::getContainerInstanceItems
 * @return
 */
const std::vector<ContainerInstanceGraphicsItem*>& NodeGraphicsItem::getContainerInstanceItems() const
{
    return container_inst_items_;
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
            return {100000, 100000};
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
    // Force this item's geometry to have the same size as the bounding rect
    QRectF adjusted_rect(rect.topLeft(), boundingRect().size());
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

    /*
    painter->setPen(QPen(Qt::red, 3));
    painter->drawRect(metadata_pixmap_item_->geometry());
    painter->setPen(QPen(Qt::green, 3));
    painter->drawRect(metadata_text_item_->geometry());
    */
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
        if (container_data == nullptr) {
            throw std::invalid_argument("NodeGraphicsItem::constructChildrenItems - ContainerInstanceData is null.");
        }
        addContainerInstanceItem(*container_data);
    }
}

/**
 * @brief NodeGraphicsItem::validateChildMove
 * @param child
 * @param pos
 */
void NodeGraphicsItem::validateChildMove(ContainerInstanceGraphicsItem* child, QPointF pos)
{
    if (child == nullptr) {
        return;
    }

    auto&& min_x = getOriginChildPos().x();
    auto&& min_y = getOriginChildPos().y();
    auto x = pos.x();
    auto y = pos.y();

    // This prevents a left/upward drag
    if (x < min_x) {
        x = min_x;
    }
    if (y < min_y) {
        y = min_y;
    }

    child->setPos(x, y);
    emit updateConnectionPos();
}

/**
 * @brief NodeGraphicsItem::toggleExpanded
 */
void NodeGraphicsItem::toggleExpanded()
{
    expanded_ = !expanded_;
    for (const auto& container_inst : container_inst_items_) {
        container_inst->setVisible(expanded_);
    }
    updateGeometry();
    emit updateConnectionPos();
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
 * @brief NodeGraphicsItem::getOriginChildPos
 * @return
 */
QPointF NodeGraphicsItem::getOriginChildPos() const
{
    return QPointF(padding, getTopRect().bottom() + padding);
}

/**
 * @brief NodeGraphicsItem::getNextChildPos
 * @return
 */
QPointF NodeGraphicsItem::getNextChildPos() const
{
    const int container_count = container_inst_items_.size();
    auto children_origin = getTopRect().bottomLeft();
    auto stack_padding = getTopRect().height() / 2.0;

    QPointF offset(stack_padding * container_count, stack_padding * container_count);
    children_origin += offset + QPointF(padding, padding);
    return children_origin;
}

/**
 * @brief NodeGraphicsItem::themeChanged
 */
void NodeGraphicsItem::themeChanged()
{
    Theme* theme = Theme::theme();
    top_color_ = theme->getActiveWidgetBorderColor();
    body_color_ = theme->getDisabledBackgroundColor();

    auto pixmap = theme->getImage("EntityIcons", "HardwareNode");
    icon_pixmap_item_->updatePixmap(pixmap);
    label_text_item_->setDefaultTextColor(theme->getTextColor());

    pixmap = Theme::theme()->getImage("Icons", "ethernet");
    metadata_pixmap_item_->updatePixmap(pixmap);
    metadata_text_item_->setDefaultTextColor(theme->getTextColor());
}

/**
 * @brief NodeGraphicsItem::setupLayout
 */
void NodeGraphicsItem::setupLayout()
{    
    icon_pixmap_item_ = new PixmapGraphicsItem(QPixmap(), this);
    icon_pixmap_item_->setPixmapPadding(pixmap_padding);

    label_text_item_ = new TextGraphicsItem(node_data_.getHostname(), this);
    label_text_item_->setTextAlignment(Qt::AlignBottom);

    metadata_text_item_ = new TextGraphicsItem(node_data_.getIP(), this);
    metadata_text_item_->setFont(QFont("Verdana", 8));
    metadata_text_item_->setTextAlignment(Qt::AlignTop);

    int sub_size = icon_size / 2.5;
    metadata_pixmap_item_ = new PixmapGraphicsItem(QPixmap(), this);
    metadata_pixmap_item_->setPixmapSquareSize(sub_size);
    metadata_pixmap_item_->setMaximumHeight(metadata_text_item_->effectiveSizeHint(Qt::PreferredSize).height());

    int info_spacing = 2;
    sub_info_layout_ = new QGraphicsLinearLayout(Qt::Horizontal);
    sub_info_layout_->setSpacing(info_spacing);
    sub_info_layout_->setContentsMargins(info_spacing, 0, 0, 0);
    sub_info_layout_->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    sub_info_layout_->addItem(metadata_pixmap_item_);
    sub_info_layout_->setAlignment(metadata_pixmap_item_, Qt::AlignHCenter | Qt::AlignTop);
    sub_info_layout_->setStretchFactor(metadata_pixmap_item_, 0);
    sub_info_layout_->addItem(metadata_text_item_);
    sub_info_layout_->setAlignment(metadata_text_item_, Qt::AlignLeft);
    sub_info_layout_->setStretchFactor(metadata_text_item_, 1);

    info_layout_ = new QGraphicsLinearLayout(Qt::Vertical);
    info_layout_->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);
    info_layout_->setSpacing(info_spacing);
    info_layout_->setContentsMargins(0, 0, 0, info_spacing);
    info_layout_->addItem(label_text_item_);
    info_layout_->addItem(sub_info_layout_);

    top_layout_ = new QGraphicsLinearLayout(Qt::Horizontal);
    top_layout_->setSpacing(top_layout_spacing);
    top_layout_->setContentsMargins(top_layout_horizontal_margin, 0, top_layout_horizontal_margin, 0);
    top_layout_->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
    top_layout_->addItem(icon_pixmap_item_);
    top_layout_->setStretchFactor(icon_pixmap_item_, 0);
    top_layout_->addItem(info_layout_);
    top_layout_->setStretchFactor(info_layout_, 1);
    top_layout_->setAlignment(info_layout_, Qt::AlignCenter);

    main_layout_ = new QGraphicsLinearLayout(Qt::Vertical);
    main_layout_->setContentsMargins(0, 0, 0, 0);
    main_layout_->setSpacing(0);
    main_layout_->addItem(top_layout_);
    main_layout_->setStretchFactor(top_layout_, 0);
    main_layout_->addStretch();

    setContentsMargins(0, 0, 0, 0);
    setLayout(main_layout_);
}