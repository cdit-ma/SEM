//
// Created by Cathlyn Aston on 19/11/20.
//

#ifndef PULSE_VIEW_NATURALANCHOR_H
#define PULSE_VIEW_NATURALANCHOR_H

#include "edgeanchor.h"
#include "../Edge/edge.h"

#include <QGraphicsObject>

namespace Pulse::View {

class NaturalAnchor : public QGraphicsObject, public EdgeAnchor {
    Q_OBJECT

public:
    explicit NaturalAnchor(QGraphicsItem* parent = nullptr);
    ~NaturalAnchor() override = default;

    void transferToAdopter(EdgeAdopter* adopter) override;
    void reclaimEdges();

    void connectEdge(Edge* edge);
    void disconnectEdges();

    [[nodiscard]] QRectF boundingRect() const override;

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

signals:
    void edgeAnchorMoved(const QPointF& pos) override;
    void edgeAnchorVisibilityChanged(bool visible) override;

private:
    QColor anchor_color_ = Qt::white;
    EdgeAdopter* active_adopter_ = nullptr;
    std::vector<Edge*> edges_;
};

} // end Pulse::View namespace
#endif // PULSE_VIEW_NATURALANCHOR_H
