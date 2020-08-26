//
// Created by Cathlyn Aston on 20/8/20.
//

#include "containerinstancegraphicsitem.h"
#include "nodegraphicsitem.h"
#include "../../../theme.h"

#include <QPainter>
#include <QGraphicsSceneMouseEvent>

const qreal pen_width = 2.0;
const int icon_size = 50;

const int padding = 15;
const int pixmap_padding = 10;
const int top_layout_spacing = 5;
const int top_layout_horizontal_margin = 10;

/**
 * @brief ContainerInstanceGraphicsItem::ContainerInstanceGraphicsItem
 * @param container_inst_data
 * @param parent
 */
ContainerInstanceGraphicsItem::ContainerInstanceGraphicsItem(const ContainerInstanceData& container_inst_data, NodeGraphicsItem* parent)
    : QGraphicsWidget(parent),
      container_inst_data_(container_inst_data)
{
    setupLayout();
    constructChildrenItems();

    setFlags(flags() | QGraphicsWidget::ItemIsMovable);
    setAcceptedMouseButtons(Qt::LeftButton);

    // This additional signal is needed for the ContainerInstance because it is explicitly show/hidden, and unlike
    // the ComponentInstance, it's not managed by a layout and hence its geometry is not updated automatically
    connect(this, &ContainerInstanceGraphicsItem::visibleChanged, this, &ContainerInstanceGraphicsItem::updateGeometry);

    connect(Theme::theme(), &Theme::theme_Changed, this, &ContainerInstanceGraphicsItem::themeChanged);
    themeChanged();
}

/**
 * @brief ContainerInstanceGraphicsItem::addComponentInstanceItem
 * @param comp_inst_data
 */
void ContainerInstanceGraphicsItem::addComponentInstanceItem(ComponentInstanceData& comp_inst_data)
{
    // Get the initial position before constructing the child graphics item
    auto child_pos = getNextChildPos();
    auto comp_inst_item = new ComponentInstanceGraphicsItem(comp_inst_data, this);
    comp_inst_item->setPos(child_pos.x(), child_pos.y());
    comp_inst_items_.push_back(comp_inst_item);

    connect(comp_inst_item, &ComponentInstanceGraphicsItem::geometryChanged, this, &ContainerInstanceGraphicsItem::updateGeometry);
    connect(comp_inst_item, &ComponentInstanceGraphicsItem::requestMove, this, &ContainerInstanceGraphicsItem::validateChildMove);
    connect(this, &ContainerInstanceGraphicsItem::updateConnectionPos, comp_inst_item, &ComponentInstanceGraphicsItem::updateConnectionPos);
}

/**
 * @brief ContainerInstanceGraphicsItem::getComponentInstanceItems
 * @return
 */
const std::vector<ComponentInstanceGraphicsItem*>& ContainerInstanceGraphicsItem::getComponentInstanceItems() const
{
    return comp_inst_items_;
}

/**
 * @brief ContainerInstanceGraphicsItem::boundingRect
 * @return
 */
QRectF ContainerInstanceGraphicsItem::boundingRect() const
{
    QRectF rect(0, 0, getWidth(), getHeight());
    auto padding = pen_width / 2.0;
    return rect.adjusted(-padding, -padding, padding, padding);
}

/**
 * @brief ContainerInstanceGraphicsItem::sizeHint
 * @param which
 * @param constraint
 * @return
 */
QSizeF ContainerInstanceGraphicsItem::sizeHint(Qt::SizeHint which, const QSizeF& constraint) const
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
 * @brief ContainerInstanceGraphicsItem::setGeometry
 * @param rect
 */
void ContainerInstanceGraphicsItem::setGeometry(const QRectF& rect)
{
    // Force this item's geometry to have the same size as the bounding rect
    QRectF adjusted_rect(rect.topLeft(), boundingRect().size());
    QGraphicsWidget::setGeometry(adjusted_rect);
}

/**
 * @brief ContainerInstanceGraphicsItem::paint
 * @param painter
 * @param option
 * @param widget
 */
void ContainerInstanceGraphicsItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
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
 * @brief ContainerInstanceGraphicsItem::mousePressEvent
 * @param event
 */
void ContainerInstanceGraphicsItem::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    if (getTopRect().contains(event->pos())) {
        prev_move_origin_ = mapToScene(event->pos());
        moving_ = true;
    }
    QGraphicsWidget::mousePressEvent(event);
}

/**
 * @brief ContainerInstanceGraphicsItem::mouseMoveEvent
 * @param event
 */
void ContainerInstanceGraphicsItem::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
    if (moving_) {
        auto cursor_pos = mapToScene(event->pos());
        auto delta = cursor_pos - prev_move_origin_;
        prev_move_origin_ = cursor_pos;
        emit requestMove(this, pos() + delta);
    }
}

/**
 * @brief ContainerInstanceGraphicsItem::mouseReleaseEvent
 * @param event
 */
void ContainerInstanceGraphicsItem::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    moving_ = false;
    prev_move_origin_ = QPointF(0, 0);
    QGraphicsWidget::mouseReleaseEvent(event);
}

/**
 * @brief ContainerInstanceGraphicsItem::mouseDoubleClickEvent
 * @param event
 */
void ContainerInstanceGraphicsItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event)
{
    if (icon_pixmap_item_->geometry().contains(event->pos())) {
        toggleExpanded();
    }
    QGraphicsWidget::mouseDoubleClickEvent(event);
}

/**
 * @brief ContainerInstanceGraphicsItem::constructChildrenItems
 * @throws std::invalid_argument
 */
void ContainerInstanceGraphicsItem::constructChildrenItems()
{
    for (const auto& comp_inst_data : container_inst_data_.getComponentInstanceData()) {
        if (comp_inst_data == nullptr) {
            throw std::invalid_argument("ContainerInstanceGraphicsItem::constructChildrenItems - ComponentInstanceData is null.");
        }
        addComponentInstanceItem(*comp_inst_data);
    }
}

/**
 * @brief ContainerInstanceGraphicsItem::toggleExpanded
 */
void ContainerInstanceGraphicsItem::toggleExpanded()
{
    expanded_ = !expanded_;
    for (const auto& comp_inst : comp_inst_items_) {
        comp_inst->setVisible(expanded_);
    }
    updateGeometry();
    emit updateConnectionPos();
}

/**
 * @brief ContainerInstanceGraphicsItem::validateChildMove
 * @param child
 * @param pos
 */
void ContainerInstanceGraphicsItem::validateChildMove(ComponentInstanceGraphicsItem* child, const QPointF& pos)
{
    if (child == nullptr) {
        return;
    }

    const auto min_x = getOriginChildPos().x();
    const auto min_y = getOriginChildPos().y();
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
 * @brief ContainerInstanceGraphicsItem::getWidth
 * @return
 */
qreal ContainerInstanceGraphicsItem::getWidth() const
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
 * @brief ContainerInstanceGraphicsItem::getHeight
 * @return
 */
qreal ContainerInstanceGraphicsItem::getHeight() const
{
    if (expanded_) {
        return getVisibleChildrenRect().bottom() + padding;
    } else {
        return getTopRect().height();
    }
}

/**
 * @brief ContainerInstanceGraphicsItem::::getTopRect
 * @return
 */
QRectF ContainerInstanceGraphicsItem::getTopRect() const
{
    return QRectF(0, 0, getWidth(), MEDEA::GraphicsLayoutItem::DEFAULT_GRAPHICS_ITEM_HEIGHT);
}

/**
 * @brief ContainerInstanceGraphicsItem::getVisibleChildrenRect
 * @return
 */
QRectF ContainerInstanceGraphicsItem::getVisibleChildrenRect() const
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
 * @brief ContainerInstanceGraphicsItem::getNextChildPos
 * @return
 */
QPointF ContainerInstanceGraphicsItem::getNextChildPos() const
{
    const int containers_count = comp_inst_items_.size();
    auto children_origin = getTopRect().bottomLeft();
    auto stack_padding = getTopRect().height() / 2.0;

    QPointF offset(stack_padding * containers_count, stack_padding * containers_count);
    children_origin += offset + QPointF(padding, padding);
    return children_origin;
}

/**
 * @brief ContainerInstanceGraphicsItem::getOriginChildPos
 * @return
 */
QPointF ContainerInstanceGraphicsItem::getOriginChildPos() const
{
    return QPointF(padding, getTopRect().bottom() + padding);
}

/**
 * @brief ContainerInstanceGraphicsItem::themeChanged
 */
void ContainerInstanceGraphicsItem::themeChanged()
{
    Theme* theme = Theme::theme();
    auto icon_name = container_inst_data_.getType() == AggServerResponse::Container::ContainerType::GENERIC ? "servers" : "docker";
    auto pixmap = theme->getImage("Icons", icon_name);

    icon_pixmap_item_->updatePixmap(pixmap);
    label_text_item_->setDefaultTextColor(theme->getTextColor());
    metadata_text_item_->setDefaultTextColor(theme->getTextColor());

    top_color_ = theme->getActiveWidgetBorderColor();
    body_color_ = theme->getAltBackgroundColor().darker(115);
    update();
}

/**
 * @brief ContainerInstanceGraphicsItem::setupLayout
 */
void ContainerInstanceGraphicsItem::setupLayout()
{
    QString docker_type = "Generic OS Process";
    QString icon_name = "servers";
    if (container_inst_data_.getType() == AggServerResponse::Container::ContainerType::DOCKER) {
        docker_type = "Docker Process";
        icon_name = "docker";
    }

    auto pix = Theme::theme()->getImage("Icons", icon_name);
    icon_pixmap_item_ = new PixmapGraphicsItem(pix, this);
    icon_pixmap_item_->setPixmapPadding(pixmap_padding);

    label_text_item_ = new TextGraphicsItem(container_inst_data_.getName(), this);
    metadata_text_item_ = new TextGraphicsItem(docker_type, this);
    metadata_text_item_->setFont(QFont("Verdana", 8));

    text_layout_ = new QGraphicsLinearLayout(Qt::Vertical);
    text_layout_->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
    text_layout_->setSpacing(0);
    text_layout_->setContentsMargins(0, 0, 0, 0);
    text_layout_->addItem(label_text_item_);
    text_layout_->addItem(metadata_text_item_);

    top_layout_ = new QGraphicsLinearLayout(Qt::Horizontal);
    top_layout_->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
    top_layout_->setSpacing(top_layout_spacing);
    top_layout_->setContentsMargins(top_layout_horizontal_margin, 0, top_layout_horizontal_margin, 0);
    top_layout_->addItem(icon_pixmap_item_);
    top_layout_->setStretchFactor(icon_pixmap_item_, 0);
    top_layout_->addItem(text_layout_);
    top_layout_->setStretchFactor(text_layout_, 1);
    top_layout_->setAlignment(text_layout_, Qt::AlignCenter);

    main_layout_ = new QGraphicsLinearLayout(Qt::Vertical);
    main_layout_->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    main_layout_->setContentsMargins(0, 0, 0, 0);
    main_layout_->setSpacing(0);
    main_layout_->addItem(top_layout_);
    main_layout_->setStretchFactor(top_layout_, 0);
    main_layout_->addStretch();

    setContentsMargins(0, 0, 0, 0);
    setLayout(main_layout_);
}