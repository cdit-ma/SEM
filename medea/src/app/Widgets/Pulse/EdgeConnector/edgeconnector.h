//
// Created by Cathlyn Aston on 24/11/20.
//

#ifndef PULSE_VIEW_EDGECONNECTOR_H
#define PULSE_VIEW_EDGECONNECTOR_H

#include "../Edge/edge.h"

#include <QGraphicsObject>

namespace Pulse::View {

class NaturalAnchor;

/**
 * @brief A source and destination EdgeConnectors are required to construct an Edge
 * An EdgeConnector can have multiple Edges attached to it and it sends geometry/visible change signals to all of them
 * The EdgeConnector is what gets passed around by the EdgeAnchors when a child EdgeAnchor gets hidden/shown
 */
class EdgeConnector : public QGraphicsObject {

    Q_OBJECT
    friend class NaturalAnchor;

public:
    void connectEdge(Edge* edge);
    void disconnectEdge(Edge* edge);

    bool isConnectedToNaturalAnchor() const;

    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;
    [[nodiscard]] QRectF boundingRect() const override;

signals:
    void positionChanged(const QPointF& pos);
    void visibilityChanged(bool visible);

public slots:
    void flashPortLifecycle();
    void unflashPortLifecycle();

private:
    explicit EdgeConnector(QGraphicsItem* parent = nullptr);
    void setNaturalAnchor(NaturalAnchor* anchor);

    NaturalAnchor* natural_anchor_ = nullptr;

    QColor default_color_;
    QColor highlight_color_;
    QColor active_color_;

    bool flashing_ = false;

    QList<Edge*> connected_edges_;
};

} // end Pulse::View namespace
#endif // PULSE_VIEW_EDGECONNECTOR_H