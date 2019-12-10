#ifndef PULSE_EDGEITEM_H
#define PULSE_EDGEITEM_H

#include "../EntityItems/portinstancegraphicsitem.h"

#include <QGraphicsObject>
#include <QPen>

namespace MEDEA {

class EdgeItem : public QGraphicsObject
{
public:
    EdgeItem(PortInstanceGraphicsItem* src, PortInstanceGraphicsItem* dst, QGraphicsItem* parent = nullptr);

    const QString& getSourceGraphmlID() const;
    const QString& getDestinationGraphmlID() const;

    void updateSourcePos();
    void updateDestinationPos();

private slots:
    void flashEdge(qint64 from_time, int flash_duration_ms);
    void unflashEdge(qint64 flash_end_time);

protected:
    // QGraphicsItem interface
    QRectF boundingRect() const override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;

private:
    void themeChanged();
    void updateEdgePath();

    QPainterPath getCubicPath(QPointF p1, QPointF p2) const;
    QPainterPath getCubicPath(QPointF p1, QPointF ctrl_p1, QPointF ctrl_p2, QPointF p2) const;

    PortInstanceGraphicsItem* src_item_ = nullptr;
    PortInstanceGraphicsItem* dst_item_ = nullptr;

    QPointF src_point_;
    QPointF dst_point_;

    QPainterPath edge_path_;
    QPainterPath arrow_;

    QPen point_pen_;
    QPen active_pen_;

    QColor default_pen_color_;
    QColor highlight_pen_color_;

    qint64 flash_end_time_ = 0;
};

}

#endif // PULSE_EDGEITEM_H
