//
// Created by Cathlyn Aston on 19/11/20.
//

#ifndef PULSE_VIEW_DELEGATEANCHOR_H
#define PULSE_VIEW_DELEGATEANCHOR_H

#include "edgeanchor.h"
#include "edgeadopter.h"

#include <QGraphicsObject>
#include <QList>

namespace Pulse::View {

class DelegateAnchor : public QGraphicsObject, public EdgeAnchor, public EdgeAdopter {
public:
    explicit DelegateAnchor(QGraphicsItem* parent = nullptr);
    ~DelegateAnchor() override = default;

    void transferToAdopter(EdgeAdopter* adopter) override;

    void adoptEdges(EdgeAnchor* anchor) override;
    void returnEdges(EdgeAnchor* anchor) override;

    void edgeAnchorMoved(const QPointF& pos) override;
    void edgeAnchorVisibilityChanged(bool visible) override;

private:
    static void checkPreCondition(EdgeAnchor* anchor);

    QList<EdgeAnchor*> attached_anchors_;
};

} // end Pulse::View namespace
#endif // PULSE_VIEW_DELEGATEANCHOR_H
