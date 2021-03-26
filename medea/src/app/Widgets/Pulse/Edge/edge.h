//
// Created by Cathlyn Aston on 19/11/20.
//

#ifndef PULSE_VIEW_EDGE_H
#define PULSE_VIEW_EDGE_H

#include "../EdgeAnchor/edgeanchor.h"

#include <QPointF>

namespace Pulse::View {

/**
 * @brief This attempts to make sure that an Edge listens to and handles its source's and
 * destination's geometry (position and size) changes and visibility state change
 */
class Edge {
public:
    virtual ~Edge() = 0;

    virtual void endPointPositionChanged() = 0;
    virtual void endPointVisibilityChanged() = 0;
};

inline Edge::~Edge() = default;

} // end Pulse::View namespace
#endif // PULSE_VIEW_EDGE_H