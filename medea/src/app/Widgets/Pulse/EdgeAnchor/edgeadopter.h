//
// Created by Cathlyn Aston on 19/11/20.
//

#ifndef PULSE_VIEW_EDGEADOPTER_H
#define PULSE_VIEW_EDGEADOPTER_H

#include "edgeanchor.h"

namespace Pulse::View {

class EdgeAdopter {
public:
    virtual ~EdgeAdopter() = 0;

    virtual void adoptEdges(EdgeAnchor* anchor) = 0;
    virtual void returnEdges(EdgeAnchor* anchor) = 0;
};

inline EdgeAdopter::~EdgeAdopter() = default;

} // end Pulse::View namespace
#endif // PULSE_VIEW_EDGEADOPTER_H
