//
// Created by Cathlyn Aston on 19/11/20.
//

#ifndef PULSE_VIEW_NATURALANCHOR_H
#define PULSE_VIEW_NATURALANCHOR_H

#include "edgeanchor.h"
#include "edgeadopter.h"
#include "../EdgeConnector/edgeconnector.h"

#include <QGraphicsObject>

namespace Pulse::View {

/**
 * @brief The NaturalAnchor is designed for Entities that have Edges connected directly to them
 * It constructs the EdgeConnector that the Edges will connect to both visually and for geometry/visible change signals
 */
class NaturalAnchor : public QGraphicsObject, public EdgeAnchor {
    Q_OBJECT

public:
    explicit NaturalAnchor(QGraphicsItem* parent = nullptr);
    ~NaturalAnchor() override = default;

    EdgeConnector& getEdgeConnector() const;

    void transferToAdopter(EdgeAdopter* adopter) override;
    void retrieveFromAdopter() override;

    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override {};

    [[nodiscard]] QRectF boundingRect() const override { return {}; };

    void triggerPositionChange(qreal x, qreal y);

signals:
    void positionChanged(const QPointF& pos) override;
    void visibilityChanged(bool visible) override;

private:
    void connectEdgeConnector();

    EdgeAdopter* active_adopter_ = nullptr;
    EdgeConnector* edge_connector_ = nullptr;
};

} // end Pulse::View namespace
#endif // PULSE_VIEW_NATURALANCHOR_H