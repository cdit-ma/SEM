//
// Created by Cathlyn Aston on 23/11/20.
//

#ifndef PULSE_VIEW_CONNECTABLE_H
#define PULSE_VIEW_CONNECTABLE_H

#include "../EdgeAnchor/naturalanchor.h"

namespace Pulse::View {

class Connectable {
public:
    virtual ~Connectable() = 0;

    virtual NaturalAnchor* getInputAnchor() = 0;
    virtual NaturalAnchor* getOutputAnchor() = 0;
};

inline Connectable::~Connectable() = default;

} // end Pulse::View namespace
#endif // PULSE_VIEW_CONNECTABLE_H
