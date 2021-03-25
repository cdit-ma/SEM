//
// Created by Cathlyn Aston on 13/11/20.
//

#ifndef PULSE_VIEW_DEFAULTEDGE_H
#define PULSE_VIEW_DEFAULTEDGE_H

#include "uuid.h"
#include "edge.h"
#include "../EdgeAnchor/naturalanchor.h"
#include "../EdgeConnector/edgeconnector.h"
#include "../../../theme.h"

#include <QGraphicsObject>
#include <QPen>

namespace Pulse::View {

class DefaultEdge : public QGraphicsObject, public Edge {
    Q_OBJECT

public:
    explicit DefaultEdge(const NaturalAnchor& src_anchor, const NaturalAnchor& dst_anchor, QGraphicsItem* parent = nullptr);
    explicit DefaultEdge(EdgeConnector& src_connector, EdgeConnector& dst_connector, QGraphicsItem* parent = nullptr);

    ~DefaultEdge() override = default;

    [[nodiscard]] re::types::Uuid getID() const;
    [[nodiscard]] QRectF boundingRect() const override;

public slots:
    void flash(qint64 from_time, qint64 flash_duration);
    void unflash(qint64 end_time);

    void flashSource();
    void unflashSource();

    void flashDestination();
    void unflashDestination();

    void endPointPositionChanged() override;
    void endPointVisibilityChanged() override;
    void endPointDestroyed() override;

private slots:
    void onSourceParentChanged(bool natural_parent);
    void onDestinationParentChanged(bool natural_parent);

    void themeChanged();

protected:
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;

private:
    EdgeConnector& getSourceConnector();
    EdgeConnector& getDestinationConnector();

    void updateEdgePath();
    static QPainterPath getCubicPath(const QPointF& p1, const QPointF& ctrl_p1, const QPointF& ctrl_p2, const QPointF& p2);

    EdgeConnector* src_connector_ = nullptr;
    EdgeConnector* dst_connector_ = nullptr;

    QPen line_pen_;
    QPen src_point_pen_;
    QPen dst_point_pen_;

    QColor default_color_;
    QColor highlight_color_;

    QColor default_point_color_;
    QColor active_src_point_color_;
    QColor active_dst_point_color_;

    QPainterPath edge_path_;
    re::types::Uuid id_;

    // This is used to prevent the flash from being stopped/reset prematurely due to previous flash timers ending
    qint64 flash_end_time_ = 0;
};

} // end Pulse::View namespace
#endif // PULSE_VIEW_DEFAULTEDGE_H