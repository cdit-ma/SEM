//
// Created by Cathlyn Aston on 23/11/20.
//

#ifndef PULSE_VIEW_CONNECTABLE_H
#define PULSE_VIEW_CONNECTABLE_H

#include "../EdgeAnchor/naturalanchor.h"

namespace Pulse::View {

/**
 * @brief All this interface does is guarantee that it has an input and an output EdgeAnchors
 * These EdgeAnchors allow the instances to be visually connected; hence the name
 */
class Connectable {
public:
    virtual ~Connectable() = 0;

    virtual EdgeAnchor* getInputAnchor() = 0;
    virtual EdgeAnchor* getOutputAnchor() = 0;
};

inline Connectable::~Connectable() = default;

} // end Pulse::View namespace
#endif // PULSE_VIEW_CONNECTABLE_H