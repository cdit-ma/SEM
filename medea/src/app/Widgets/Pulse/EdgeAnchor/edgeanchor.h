//
// Created by Cathlyn Aston on 20/10/20.
//

#ifndef PULSE_VIEW_EDGEANCHOR_H
#define PULSE_VIEW_EDGEANCHOR_H

#include <QPointF>

namespace Pulse::View {

class EdgeAdopter;

class EdgeAnchor {
public:
    virtual ~EdgeAnchor() = 0;

    virtual void transferToAdopter(EdgeAdopter* adopter) = 0;

    virtual void edgeAnchorMoved(const QPointF& pos) = 0;
    virtual void edgeAnchorVisibilityChanged(bool visible) = 0;
};

inline EdgeAnchor::~EdgeAnchor() = default;

} // end Pulse::View namespace
#endif // PULSE_VIEW_EDGEANCHOR_H
