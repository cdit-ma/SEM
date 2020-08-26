#include "componentinstancegraphicsitem.h"
#include "containerinstancegraphicsitem.h"
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
ComponentInstanceGraphicsItem::ComponentInstanceGraphicsItem(const ComponentInstanceData& comp_inst_data, ContainerInstanceGraphicsItem* parent)
    : QGraphicsWidget(parent),
      comp_inst_data_(comp_inst_data)
{
    setupLayout();
    constructChildrenItems();

    setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    setFlags(flags() | QGraphicsWidget::ItemIsMovable);
    setAcceptedMouseButtons(Qt::LeftButton);

    connect(Theme::theme(), &Theme::theme_Changed, this, &ComponentInstanceGraphicsItem::themeChanged);
    themeChanged();
}

/**
 * @brief ComponentInstanceGraphicsItem::getGraphmlID
 * @return
 */
const QString& ComponentInstanceGraphicsItem::getGraphmlID() const
{
    return comp_inst_data_.getGraphmlID();
}

/**
 * @brief ComponentInstanceGraphicsItem::addPortInstanceItem
 * @param port_data
 */
void ComponentInstanceGraphicsItem::addPortInstanceItem(PortInstanceData& port_data)
{
    auto port_inst_item = new PortInstanceGraphicsItem(port_data, this);
    connect(this, &ComponentInstanceGraphicsItem::updateConnectionPos, port_inst_item, &PortInstanceGraphicsItem::updateConnectionPos);
    port_inst_items_.push_back(port_inst_item);

    auto&& port_kind = port_inst_item->getPortKind();
    if (port_kind == AggServerResponse::Port::Kind::PUBLISHER || port_kind == AggServerResponse::Port::Kind::REQUESTER) {
        auto&& alignment = Qt::AlignRight;
        children_layout_->addItem(port_inst_item, right_port_count_, 2, alignment);
        right_port_count_++;
    } else {
        auto&& alignment = Qt::AlignLeft;
        children_layout_->addItem(port_inst_item, left_port_count_, 0, alignment);
        left_port_count_++;
    }
}

/**
 * @brief ComponentInstanceGraphicsItem::getPortInstanceItems
 * @return
 */
const std::vector<PortInstanceGraphicsItem*>& ComponentInstanceGraphicsItem::getPortInstanceItems() const
{
    return port_inst_items_;
}

/**
 * @brief ComponentInstanceGraphicsItem::addWorkerInstanceItem
 * @param worker_inst_data
 */
void ComponentInstanceGraphicsItem::addWorkerInstanceItem(WorkerInstanceData& worker_inst_data)
{
    auto worker_inst_item = new WorkerInstanceGraphicsItem(worker_inst_data, this);
    worker_inst_items_.push_back(worker_inst_item);

    children_layout_->addItem(worker_inst_item, worker_count, 1, Qt::AlignCenter);
    worker_count++;
}

/**
 * @brief ComponentInstanceGraphicsItem::getWorkerInstanceItems
 * @return
 */
const std::vector<WorkerInstanceGraphicsItem*>& ComponentInstanceGraphicsItem::getWorkerInstanceItems() const
{
    return worker_inst_items_;
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
        emit requestMove(this, pos() + delta);
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
    if (icon_pixmap_item_->geometry().contains(event->pos())) {
        toggleExpanded();
    }
    QGraphicsWidget::mouseDoubleClickEvent(event);
}

/**
 * @brief ComponentInstanceGraphicsItem::constructChildrenItems
 * @throws std::invalid_argument
 */
void ComponentInstanceGraphicsItem::constructChildrenItems()
{
    for (const auto& port_data : comp_inst_data_.getPortInstanceData()) {
        if (port_data == nullptr) {
            throw std::invalid_argument("ComponentInstanceGraphicsItem::constructChildrenItems - PortInstanceData is null.");
        }
        addPortInstanceItem(*port_data);
    }
    for (const auto& worker_inst_data : comp_inst_data_.getWorkerInstanceData()) {
        if (worker_inst_data == nullptr) {
            throw std::invalid_argument("ComponentInstanceGraphicsItem::constructChildrenItems - WorkerInstanceData is null.");
        }
        addWorkerInstanceItem(*worker_inst_data);
    }
}

/**
 * @brief ComponentInstanceGraphicsItem::toggleExpanded
 */
void ComponentInstanceGraphicsItem::toggleExpanded()
{
    expanded_ = !expanded_;
    for (const auto& port_inst : port_inst_items_) {
        port_inst->setVisible(expanded_);
    }
    for (const auto& worker_inst : worker_inst_items_) {
        worker_inst->setVisible(expanded_);
    }
    emit updateConnectionPos();
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

    top_layout_ = new QGraphicsLinearLayout(Qt::Horizontal);
    top_layout_->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
    top_layout_->setSpacing(top_layout_spacing);
    top_layout_->setContentsMargins(top_layout_horizontal_margin, 0, top_layout_horizontal_margin, 0);
    top_layout_->addItem(icon_pixmap_item_);
    top_layout_->setStretchFactor(icon_pixmap_item_, 0);
    top_layout_->addItem(label_text_item_);
    top_layout_->setStretchFactor(label_text_item_, 1);

    auto spacing = MEDEA::GraphicsLayoutItem::DEFAULT_GRAPHICS_ITEM_HEIGHT * 0.5;
    auto margin = spacing / 3;
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

    setContentsMargins(0, 0, 0, 0);
    setLayout(main_layout_);
}