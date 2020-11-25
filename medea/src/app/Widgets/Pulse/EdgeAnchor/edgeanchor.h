//
// Created by Cathlyn Aston on 20/10/20.
//

#ifndef PULSE_VIEW_EDGEANCHOR_H
#define PULSE_VIEW_EDGEANCHOR_H

#include <QPointF>

namespace Pulse::View {

class EdgeAdopter;

/**
 * @brief An EdgeAnchor is designed to lock one or more EdgeConnectors in place
 * That is, the EdgeConnectors will move with the EdgeAnchor that it is actively attached to
 * When hidden, it will transfer its attached EdgeConnectors to the given EdgeAdopter and become inactive
 * When re-shown, it will become active again and continue to send signals to those same EdgeConnectors
 */
class EdgeAnchor {
public:
    virtual ~EdgeAnchor() = 0;

    virtual void transferToAdopter(EdgeAdopter* adopter) = 0;
    virtual void retrieveFromAdopter() = 0;

    virtual void edgeAnchorMoved(const QPointF& pos) = 0;
    virtual void edgeAnchorVisibilityChanged(bool visible) = 0;
};

inline EdgeAnchor::~EdgeAnchor() = default;

} // end Pulse::View namespace
#endif // PULSE_VIEW_EDGEANCHOR_H