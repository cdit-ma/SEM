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
    explicit DefaultEdge(const EdgeConnector& src_connector, const EdgeConnector& dst_connector, QGraphicsItem* parent = nullptr);

    ~DefaultEdge() override = default;

    [[nodiscard]] re::types::Uuid getID() const;
    [[nodiscard]] QRectF boundingRect() const override;

public slots:
    void onSourceMoved(const QPointF& pos) override;
    void onDestinationMoved(const QPointF& pos) override;

    void onSourceVisibilityChanged(bool visible) override;
    void onDestinationVisibilityChanged(bool visible) override;

    void onSourceParentChanged(bool natural_parent);
    void onDestinationParentChanged(bool natural_parent);

    void themeChanged();

protected:
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;

private:
    void updateEdgePath();
    static QPainterPath getCubicPath(const QPointF& p1, const QPointF& ctrl_p1, const QPointF& ctrl_p2, const QPointF& p2);

    struct EdgeConnectorProperties {
        QPointF pos;
        QPen pen;
        bool visible = true;
    };

    EdgeConnectorProperties src_properties_;
    EdgeConnectorProperties dst_properties_;

    QPen line_pen_;
    QColor default_point_color_;

    QPainterPath edge_path_;
    re::types::Uuid id_;
};

} // end Pulse::View namespace
#endif // PULSE_VIEW_DEFAULTEDGE_H