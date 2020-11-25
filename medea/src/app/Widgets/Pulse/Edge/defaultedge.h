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
    explicit DefaultEdge(NaturalAnchor& src_anchor, NaturalAnchor& dst_anchor, QGraphicsItem* parent = nullptr);
    explicit DefaultEdge(EdgeConnector& src_connector, EdgeConnector& dst_connector, QGraphicsItem* parent = nullptr);

    ~DefaultEdge() override = default;

    [[nodiscard]] re::types::Uuid getID() const;
    [[nodiscard]] QRectF boundingRect() const override;

public slots:
    void onSourceMoved(const QPointF& pos) override;
    void onDestinationMoved(const QPointF& pos) override;

    void onSourceVisibilityChanged(bool visible) override;
    void onDestinationVisibilityChanged(bool visible) override;

    void themeChanged();

protected:
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;

private:
    void updateEdgePath();
    static QPainterPath getCubicPath(const QPointF& p1, const QPointF& ctrl_p1, const QPointF& ctrl_p2, const QPointF& p2);

    QPen line_pen_;
    QPen point_pen_;
    re::types::Uuid id_;

    QPointF src_pos_;
    QPointF dst_pos_;
    QPainterPath edge_path_;

    bool src_visible_ = true;
    bool dst_visible_ = true;
};

} // end Pulse::View namespace
#endif // PULSE_VIEW_DEFAULTEDGE_H