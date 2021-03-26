//
// Created by Cathlyn Aston on 19/11/20.
//

#ifndef PULSE_VIEW_EDGEADOPTER_H
#define PULSE_VIEW_EDGEADOPTER_H

#include "edgeanchor.h"
#include "../EdgeConnector/edgeconnector.h"

namespace Pulse::View {

/**
 * @brief This interface is designed to take over the connections from a given EdgeAnchor to its attached EdgeConnector
 * Adopting an EdgeAnchor adds its EdgeConnector to the list that will be receiving signals from an instance of
 * this interface, while as returning the EdgeConnector(s) to an EdgeAnchor removes it from the same list
 */
class EdgeAdopter {
public:
    virtual ~EdgeAdopter() = 0;

    virtual void adoptEdges(EdgeAnchor* anchor, EdgeConnector* connector) = 0;
    virtual void returnEdges(EdgeAnchor& anchor) = 0;
};

inline EdgeAdopter::~EdgeAdopter() = default;

} // end Pulse::View namespace
#endif // PULSE_VIEW_EDGEADOPTER_H