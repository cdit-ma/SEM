//
// Created by Cathlyn Aston on 13/11/20.
//

#ifndef PULSE_VIEW_DEFAULTEDGE_H
#define PULSE_VIEW_DEFAULTEDGE_H

#include "uuid.h"
#include "edge.h"
#include "../EdgeAnchor/naturalanchor.h"

#include <QGraphicsObject>
#include <QPen>

namespace Pulse::View {

class DefaultEdge : public QGraphicsObject, public Edge {
    Q_OBJECT

public:
    explicit DefaultEdge(NaturalAnchor* src_anchor, NaturalAnchor* dst_anchor, QGraphicsItem* parent = nullptr);
    ~DefaultEdge() override = default;

    [[nodiscard]] re::types::Uuid getID() const;
    [[nodiscard]] QRectF boundingRect() const override;

public slots:
    void onSourceMoved(const QPointF& pos) override;
    void onDestinationMoved(const QPointF& pos) override;

    void onSourceVisibilityChanged(bool visible) override;
    void onDestinationVisibilityChanged(bool visible) override;

protected:
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;

private:
    QPen pen_;
    re::types::Uuid id_;

    QPointF source_pos_;
    QPointF destination_pos_;

    NaturalAnchor* natural_src_anchor_ = nullptr;
    NaturalAnchor* natural_dst_anchor_ = nullptr;
};

} // end Pulse::View namespace
#endif // PULSE_VIEW_DEFAULTEDGE_H