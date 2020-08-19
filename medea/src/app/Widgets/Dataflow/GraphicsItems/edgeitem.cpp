#include "edgeitem.h"
#include "../../../theme.h"

#include <QPainter>
#include <QTimer>

const qreal pen_width = 2.0;
const qreal highlight_pen_width = pen_width * 1.15;

const qreal min_offset = 40.0;
const qreal max_offset = 60.0;

using namespace MEDEA;

/**
 * @brief EdgeItem::EdgeItem
 * @param src
 * @param dst
 * @param parent
 * @throws std::invalid_argument
 */
EdgeItem::EdgeItem(PortInstanceGraphicsItem* src, PortInstanceGraphicsItem* dst, QGraphicsItem* parent)
    : QGraphicsObject(parent)
{
    if (src) {
        src_item_ = src;
    } else {
        throw std::invalid_argument("Cannot construct EdgeItem - Source item is null.");
    }
    if (dst) {
        dst_item_ = dst;
    } else {
        throw std::invalid_argument("Cannot construct EdgeItem - Destination item is null.");
    }

    connect(src, &PortInstanceGraphicsItem::updateConnectionPos, this, &EdgeItem::updateSourcePos);
    connect(src, &PortInstanceGraphicsItem::visibleChanged, this, &EdgeItem::updateVisibility);
    connect(src, &PortInstanceGraphicsItem::flashEdge, this, &EdgeItem::flashEdge);

    connect(dst, &PortInstanceGraphicsItem::updateConnectionPos, this, &EdgeItem::updateDestinationPos);
    connect(dst, &PortInstanceGraphicsItem::visibleChanged, this, &EdgeItem::updateVisibility);
    if (dst->getPortKind() == AggServerResponse::Port::Kind::REPLIER) {
        connect(dst, &PortInstanceGraphicsItem::flashEdge, this, &EdgeItem::flashEdge);
    }

    connect(Theme::theme(), &Theme::theme_Changed, this, &EdgeItem::themeChanged);
    themeChanged();
}

/**
 * @brief EdgeItem::getSourceGraphmlID
 * @return
 */
const QString& EdgeItem::getSourceGraphmlID() const
{
    return src_item_->getGraphmlID();
}

/**
 * @brief EdgeItem::getDestinationGraphmlID
 * @return
 */
const QString& EdgeItem::getDestinationGraphmlID() const
{
    return dst_item_->getGraphmlID();
}

/**
 * @brief EdgeItem::updateSourcePos
 */
void EdgeItem::updateSourcePos()
{
    src_point_ = src_item_->getEdgePoint();
    updateEdgePath();
}

/**
 * @brief EdgeItem::updateDestinationPos
 */
void EdgeItem::updateDestinationPos()
{
    dst_point_ = dst_item_->getEdgePoint();
    updateEdgePath();
}

/**
 * @brief EdgeItem::boundingRect
 * @return
 */
QRectF EdgeItem::boundingRect() const
{
    QRectF rect = edge_path_.boundingRect();
    auto half_pen_width = active_pen_.widthF();
    return rect.adjusted(-half_pen_width, -half_pen_width, half_pen_width, half_pen_width);
}

/**
 * @brief EdgeItem::paint
 * @param painter
 * @param option
 * @param widget
 */
void EdgeItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    Q_UNUSED(widget);
    Q_UNUSED(option);

    painter->strokePath(edge_path_, active_pen_);

    painter->setPen(point_pen_);
    painter->drawPoint(src_point_);
    painter->drawPoint(dst_point_);
}

/**
 * @brief EdgeItem::flashEdge
 * @param from_time
 * @param flash_duration_ms
 */
void EdgeItem::flashEdge(qint64 from_time, int flash_duration_ms)
{
    if (flash_duration_ms <= 0) {
        return;
    }

    // NOTE: We can add an edge width multiplier here to vary the edge flash width based on how busy it is
    //active_pen_.setWidthF(highlight_pen_width * edge_width_multiplier);

    // Switch pen color
    point_pen_.setColor(highlight_pen_color_);
    active_pen_.setColor(highlight_pen_color_);
    update();

    // This function can be called multiple times by multiple callers
    // Due to this, the flash end time needs to be stored and updated to avoid the flash being stopped prematurely
    auto&& end_time = from_time + flash_duration_ms;
    flash_end_time_ = qMax(end_time, flash_end_time_);

    QTimer::singleShot(flash_duration_ms, this, [this, end_time]() {
        unflashEdge(end_time);
    });
}

/**
 * @brief EdgeItem::unflashEdge
 * @param flash_end_time
 */
void EdgeItem::unflashEdge(qint64 flash_end_time)
{
    // Reset the pen color when we've reached the flash end time
    if (flash_end_time_ == flash_end_time) {
        point_pen_.setColor(default_pen_color_);
        active_pen_.setColor(default_pen_color_);
        active_pen_.setWidthF(pen_width);
        update();
        flash_end_time_ = 0;
    }
}

/**
 * @brief EdgeItem::themeChanged
 */
void EdgeItem::themeChanged()
{
    Theme* theme = Theme::theme();
    default_pen_color_ = theme->getAltTextColor();
    highlight_pen_color_ = theme->getHighlightColor();
    active_pen_ = QPen(default_pen_color_, pen_width);
    point_pen_ = QPen(default_pen_color_, pen_width * 3, Qt::SolidLine, Qt::SquareCap);
    update();
}

/**
 * @brief EdgeItem::updateEdgePath
 * This updates the painter path that is used to draw the dege
 * It needs to be called every time the src or dst points are changed
 */
void EdgeItem::updateEdgePath()
{
    auto dx = dst_point_.x() - src_point_.x();
    auto dy = dst_point_.y() - src_point_.y();

    // Need to get the abs of dx because we are setting the direction of the horizontal offset
    auto x_offset = qMin(max_offset, min_offset + qAbs(dx) / 25.0);

    // Not getting the abs of dy allows its sign to determine the vertical offset's direction
    auto y_offset = dy / 10.0;

    // Get the midpoint
    QPointF mid_point = (src_point_ + dst_point_) / 2.0;

    QPointF fixed_ctr1 = src_point_ + QPointF(x_offset, 0);
    QPointF fixed_ctr2 = dst_point_ - QPointF(x_offset, 0);

    QPointF ctrl_p1 = fixed_ctr1 + QPointF(qAbs(y_offset), y_offset);
    QPointF ctrl_p2 = fixed_ctr2 - QPointF(qAbs(y_offset), y_offset);

    auto&& src_path = getCubicPath(src_point_, fixed_ctr1, ctrl_p1, mid_point);
    auto&& dst_path = getCubicPath(dst_point_, fixed_ctr2, ctrl_p2, mid_point);

    edge_path_ = src_path;
    edge_path_.addPath(dst_path);

    prepareGeometryChange();
    update();
}

/**
 * @brief EdgeItem::updateVisibility
 * This checks if at least one of the top-most parent item for either the src or dst is visible.
 * If there is no parent item visible, hide the edge item
 */
void EdgeItem::updateVisibility()
{    
    QGraphicsItem* src_top_visible_parent = src_item_->parentItem();
    bool src_visible = src_item_->isVisible();
    if (!src_visible) {
        while (src_top_visible_parent != nullptr) {
            if (src_top_visible_parent->isVisible()) {
                src_visible = true;
                break;
            }
            src_top_visible_parent = src_top_visible_parent->parentItem();
        }
    }

    QGraphicsItem* dst_top_visible_parent = dst_item_->parentItem();
    bool dst_visible = dst_item_->isVisible();
    if (!dst_visible) {
        while (dst_top_visible_parent != nullptr) {
            if (dst_top_visible_parent->isVisible()) {
                dst_visible = true;
                break;
            }
            dst_top_visible_parent = dst_top_visible_parent->parentItem();
        }
    }

    // If both the top-most src and dst items are not null and are the same, hide the edge item
    if (src_top_visible_parent && dst_top_visible_parent && (src_top_visible_parent == dst_top_visible_parent)) {
        setVisible(false);
    } else {
        setVisible(src_visible || dst_visible);
    }
}

/**
 * @brief EdgeItem::getCubicPath
 * @param p1
 * @param p2
 * @return
 */
QPainterPath EdgeItem::getCubicPath(QPointF p1, QPointF p2) const
{
    qreal mid_x = (p1.x() + p2.x()) / 2;
    QPointF ctrl_p1(mid_x, p1.y());
    QPointF ctrl_p2(mid_x, p2.y());
    return getCubicPath(p1, ctrl_p1, ctrl_p2, p2);
}

/**
 * @brief EdgeItem::getCubicPath
 * @param p1
 * @param ctrl_p1
 * @param ctrl_p2
 * @param p2
 * @return
 */
QPainterPath EdgeItem::getCubicPath(QPointF p1, QPointF ctrl_p1, QPointF ctrl_p2, QPointF p2) const
{
    QPainterPath path(p1);
    path.cubicTo(ctrl_p1, ctrl_p2, p2);
    return path;
}