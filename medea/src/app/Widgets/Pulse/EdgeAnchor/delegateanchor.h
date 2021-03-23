//
// Created by Cathlyn Aston on 19/11/20.
//

#ifndef PULSE_VIEW_DELEGATEANCHOR_H
#define PULSE_VIEW_DELEGATEANCHOR_H

#include "edgeanchor.h"
#include "edgeadopter.h"

#include <QGraphicsObject>

namespace Pulse::View {

/**
 * @brief The DelegateAnchor is designed to be able to adopt EdgeConnectors from other EdgeAnchors
 * When a child of the parent item that this DelegateAnchor is owned by has been hidden, the child's EdgeAnchor(s)
 * transfers the connections to its EdgeConnectors to this DelegateAnchor. This means that the EdgeConnectors now
 * listen and respond to the signals and slots triggered by the DelegateAnchor they've been transferred to.
 * These connections to the EdgeConnectors are returned to the passing child's EdgeAnchor when the child is re-shown.
 */
class DelegateAnchor : public QGraphicsObject, public EdgeAnchor, public EdgeAdopter {
    Q_OBJECT

public:
    explicit DelegateAnchor(QGraphicsItem* parent = nullptr);
    ~DelegateAnchor() override = default;

    void transferToAdopter(EdgeAdopter* adopter) override;
    void retrieveFromAdopter() override;

    void adoptEdges(EdgeAnchor* anchor, EdgeConnector* connector) override;
    void returnEdges(EdgeAnchor& anchor) override;

    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override {};

    [[nodiscard]] QRectF boundingRect() const override { return {}; };

    void triggerPositionChange(qreal x, qreal y);

signals:
    void positionChanged(const QPointF& pos) override;
    void visibilityChanged(bool visible) override;

private:
    void connectEdgeConnector(EdgeConnector* connector);
    void disconnectEdgeConnector(EdgeConnector* connector);
    void disconnectEdgeConnectors();
    void reconnectEdgeConnectors();

    EdgeAdopter* active_adopter_ = nullptr;
    QMultiHash<EdgeAnchor*, EdgeConnector*> attached_connectors_;
};

} // end Pulse::View namespace
#endif // PULSE_VIEW_DELEGATEANCHOR_H