#ifndef EDGEITEM_H
#define EDGEITEM_H

#include "../EntityItems/portinstancegraphicsitem.h"

#include <QGraphicsItem>
#include <QPen>

namespace MEDEA {

class EdgeItem : public QGraphicsItem
{
public:
    EdgeItem(PortInstanceGraphicsItem* src, PortInstanceGraphicsItem* dst, QGraphicsItem* parent = nullptr);

    void updateSourcePos();
    void updateDestinationPos();

protected:
    // QGraphicsItem interface
    QRectF boundingRect() const override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;

private:
    void flashEdge();
    void updateEdge();

    QPainterPath getArrowPath(QPointF arrow_point) const;
    QPainterPath getCubicPath(QPointF p1, QPointF ctrl_p1, QPointF ctrl_p2, QPointF p2) const;

    PortInstanceGraphicsItem* src_item_ = nullptr;
    PortInstanceGraphicsItem* dst_item_ = nullptr;

    QPointF src_point_;
    QPointF dst_point_;

    QPainterPath edge_path_;
    QPainterPath arrow_;

    QPen point_pen_;
    QPen active_pen_;
    QPen default_pen_;
    QPen highlight_pen_;
};

}

#endif // EDGEITEM_H
