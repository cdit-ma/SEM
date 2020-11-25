//
// Created by Cathlyn Aston on 13/11/20.
//

#include "defaultedge.h"

#include <QPainter>

using namespace Pulse::View;

const qreal pen_width = 2.0;
const qreal min_offset = 40.0;
const qreal max_offset = 60.0;

/**
 * @brief DefaultEdge::DefaultEdge
 * @param src_anchor
 * @param dst_anchor
 * @param parent
 */
DefaultEdge::DefaultEdge(NaturalAnchor& src_anchor, NaturalAnchor& dst_anchor, QGraphicsItem* parent)
    : DefaultEdge(src_anchor.getEdgeConnector(), dst_anchor.getEdgeConnector(), parent) {}

/**
 * DefaultEdge::DefaultEdge
 * @param src_connector
 * @param dst_connector
 * @param parent
 */
DefaultEdge::DefaultEdge(EdgeConnector& src_connector, EdgeConnector& dst_connector, QGraphicsItem* parent)
    : QGraphicsObject(parent)
{
    src_pos_ = src_connector.scenePos();
    connect(&src_connector, &EdgeConnector::positionChanged, this, &DefaultEdge::onSourceMoved);
    connect(&src_connector, &EdgeConnector::visibilityChanged, this, &DefaultEdge::onSourceVisibilityChanged);

    dst_pos_ = dst_connector.scenePos();
    connect(&dst_connector, &EdgeConnector::positionChanged, this, &DefaultEdge::onDestinationMoved);
    connect(&dst_connector, &EdgeConnector::visibilityChanged, this, &DefaultEdge::onDestinationVisibilityChanged);

    connect(Theme::theme(), &Theme::theme_Changed, this, &DefaultEdge::themeChanged);
    themeChanged();
}

/**
 * @brief DefaultEdge::getID
 * @return
 */
re::types::Uuid DefaultEdge::getID() const
{
    return id_;
}

/**
 * @brief DefaultEdge::boundingRect
 * @return
 */
QRectF DefaultEdge::boundingRect() const
{
    auto half_pen_width = line_pen_.widthF() / 2;
    return edge_path_.boundingRect().adjusted(-half_pen_width, -half_pen_width, half_pen_width, half_pen_width);
}

/**
 * @brief DefaultEdge::onSourceMoved
 * @param pos
 */
void DefaultEdge::onSourceMoved(const QPointF& pos)
{
    src_pos_ = pos;
    updateEdgePath();
}

/**
 * @brief DefaultEdge::onDestinationMoved
 * @param pos
 */
void DefaultEdge::onDestinationMoved(const QPointF& pos)
{
    dst_pos_ = pos;
    updateEdgePath();
}

/**
 * @brief DefaultEdge::onSourceVisibilityChanged
 * @param visible
 */
void DefaultEdge::onSourceVisibilityChanged(bool visible)
{
    // check that both of the src and dst is visible; if not, hide the edge
    src_visible_ = visible;
    setVisible(src_visible_ && dst_visible_);
}

/**
 * @brief DefaultEdge::onDestinationVisibilityChanged
 * @param visible
 */
void DefaultEdge::onDestinationVisibilityChanged(bool visible)
{
    // check that both of the src and dst is visible; if not, hide the edge
    dst_visible_ = visible;
    setVisible(src_visible_ && dst_visible_);
}

/**
 * @brief DefaultEdge::themeChanged
 */
void DefaultEdge::themeChanged()
{
    auto theme = Theme::theme();
    line_pen_ = QPen(theme->getTextColor(), pen_width, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
    point_pen_ = QPen(theme->getTextColor(ColorRole::SELECTED), pen_width * 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
    update();
}

/**
 * @brief DefaultEdge::paint
 * @param painter
 * @param option
 * @param widget
 */
void DefaultEdge::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    Q_UNUSED(widget);
    Q_UNUSED(option);

    painter->strokePath(edge_path_, line_pen_);
    painter->setPen(point_pen_);
    painter->drawPoint(src_pos_);
    painter->drawPoint(dst_pos_);
}

/**
 * @brief DefaultEdge::updateEdgePath
 */
void DefaultEdge::updateEdgePath()
{
    auto dx = dst_pos_.x() - src_pos_.x();
    auto dy = dst_pos_.y() - src_pos_.y();

    // Need to get the abs of dx because we are setting the direction of the horizontal offset
    auto x_offset = qMin(max_offset, min_offset + qAbs(dx) / 25.0);

    // Not getting the abs of dy allows its sign to determine the vertical offset's direction
    auto y_offset = dy / 10.0;

    // Get the midpoint
    QPointF mid_point = (src_pos_ + dst_pos_) / 2.0;

    QPointF fixed_ctr1 = src_pos_ + QPointF(x_offset, 0);
    QPointF fixed_ctr2 = dst_pos_ - QPointF(x_offset, 0);

    QPointF ctrl_p1 = fixed_ctr1 + QPointF(qAbs(y_offset), y_offset);
    QPointF ctrl_p2 = fixed_ctr2 - QPointF(qAbs(y_offset), y_offset);

    auto&& src_path = getCubicPath(src_pos_, fixed_ctr1, ctrl_p1, mid_point);
    auto&& dst_path = getCubicPath(dst_pos_, fixed_ctr2, ctrl_p2, mid_point);

    edge_path_ = src_path;
    edge_path_.addPath(dst_path);

    prepareGeometryChange();
    update();
}

/**
 * @brief DefaultEdge::getCubicPath
 * @param p1
 * @param ctrl_p1
 * @param ctrl_p2
 * @param p2
 * @return
 */
QPainterPath DefaultEdge::getCubicPath(const QPointF& p1,
                                       const QPointF& ctrl_p1,
                                       const QPointF& ctrl_p2,
                                       const QPointF& p2)
{
    QPainterPath path(p1);
    path.cubicTo(ctrl_p1, ctrl_p2, p2);
    return path;
}