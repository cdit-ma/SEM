#include "edgeitem.h"

#include <QPainter>
#include <QTimer>
#include <QtConcurrent>
#include <QStyleOptionGraphicsItem>

const qreal pen_width = 2.0;
const qreal arrow_length = 4.0;

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
EdgeItem::EdgeItem(PortInstanceGraphicsItem *src, PortInstanceGraphicsItem *dst, QGraphicsItem* parent)
    : QGraphicsItem(parent)
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

    point_pen_ = QPen(Qt::gray, pen_width * 3, Qt::SolidLine, Qt::SquareCap);
    highlight_pen_ = QPen(Qt::red, pen_width * 1.5); //, Qt::DashDotLine);
    default_pen_ = QPen(Qt::gray, pen_width);
    active_pen_ = default_pen_;

    // This initialises the src & dst points and the edge path
    updateSourcePos();
    updateDestinationPos();

    //setZValue(-1);
    //flashEdge();
}


/**
 * @brief EdgeItem::updateSourcePos
 */
void EdgeItem::updateSourcePos()
{
    auto icon_rect = src_item_->getIconSceneRect();
    src_point_.setX(icon_rect.right());
    src_point_.setY(icon_rect.center().y());
    updateEdge();
}


/**
 * @brief EdgeItem::updateDestinationPos
 */
void EdgeItem::updateDestinationPos()
{
    auto icon_rect = dst_item_->getIconSceneRect();
    dst_point_.setX(icon_rect.left());
    dst_point_.setY(icon_rect.center().y());
    updateEdge();
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
void EdgeItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(widget);
    Q_UNUSED(option);

    painter->strokePath(edge_path_, active_pen_);

    painter->setPen(point_pen_);
    painter->drawPoint(src_point_);
    painter->drawPoint(dst_point_);

    //painter->setBrush(active_pen_.color());
    //painter->drawPath(arrow_);
}


/**
 * @brief EdgeItem::flashEdge
 */
void EdgeItem::flashEdge()
{
    QtConcurrent::run([this]() {
        // Switch pen
        int flash_count = 50;
        while (--flash_count > 0) {
            active_pen_ = (flash_count % 2 == 0) ? highlight_pen_ : default_pen_;
            point_pen_.setColor(active_pen_.color());
            update();
            QThread::currentThread()->msleep(250);
        }
    });
}


/**
 * @brief EdgeItem::updateEdge
 * This updates the painter path that is used to draw the dege
 * It needs to be called every time the src or dst points are changed
 */
void EdgeItem::updateEdge()
{
    auto dx = dst_point_.x() - src_point_.x();
    auto dy = dst_point_.y() - src_point_.y();

    // Need to get the abs of dx because we are setting the direction of the horizontal offset
    auto x_offset = qMin(max_offset, min_offset + qAbs(dx) / 25.0);

    // Not getting the abs of dy allows its sign to determine the vertical offset's direction
    auto y_offset = dy / 8.0;

    /*
    // TODO - Calculate the offset so that when the points are close enough together the control points aren't so far out

    auto src_x = src_point_.x();
    auto src_y = src_point_.y();
    auto dst_x = dst_point_.x();
    auto dst_y = dst_point_.y();

    // Distance between src and dst
    auto distance = sqrt(pow(dst_x - src_x, 2) + pow(dst_y - src_y, 2));
    auto offset = distance / 10.0;
    */

    // Get the midpoint
    QPointF mid_point = (src_point_ + dst_point_) / 2.0;

    QPointF fixed_ctr1 = src_point_ + QPointF(x_offset, 0);
    QPointF fixed_ctr2 = dst_point_ - QPointF(x_offset, 0);

    QPointF ctrl_p1 = QPointF(fixed_ctr1.x() + x_offset, src_point_.y() + y_offset);
    QPointF ctrl_p2 = QPointF(fixed_ctr2.x() - x_offset, dst_point_.y() - y_offset);

    auto src_path = getCubicPath(src_point_, fixed_ctr1, ctrl_p1, mid_point);
    auto dst_path = getCubicPath(dst_point_, fixed_ctr2, ctrl_p2, mid_point);
    edge_path_ = src_path + dst_path;

    // Calculate the arrow
    arrow_ = getArrowPath(mid_point);

    prepareGeometryChange();
    update();
}


/**
 * @brief EdgeItem::getArrowPath
 * @param arrow_point
 * @return
 */
QPainterPath EdgeItem::getArrowPath(QPointF arrow_point) const
{
    int x = arrow_length;

    QPointF start_point = arrow_point - QPointF(x, 0);
    QPointF top_point = start_point + QPointF(x, -arrow_length / 2);
    QPointF bottom_point = start_point + QPointF(x, arrow_length / 2);

    QPainterPath path;
    path.moveTo(start_point);
    path.lineTo(top_point);
    path.lineTo(bottom_point);
    path.lineTo(start_point);
    return path;
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
    QPainterPath path;
    path.moveTo(p1);
    path.cubicTo(ctrl_p1, ctrl_p2, p2);
    return path;
}
